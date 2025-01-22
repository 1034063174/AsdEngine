#include <ntifs.h>
#include "Utils/CppSupport.h"
#include "etwhook_init.hpp"
#include "etwhook_manager.hpp"
#include"Utils/MacroHelper.h"
#include <wdm.h>
#include"comm/IoComm.h"

#define SHARED_MEMORY_NAME L"\\BaseNamedObjects\\SharedMemory"  // 共享内存名称

void* g_section_object;
HANDLE g_pid = 0;  // 共享内存指针
PVOID g_SharedMemory = NULL;  // 共享内存指针
HANDLE g_SharedMemorySection = NULL;  // 共享内存段句柄
HANDLE g_kernelModeAddress1 = NULL;  // 
PVOID g_kernelModeAddress = NULL;  // 内核内存指针
PMDL g_Mdl = NULL; // MDL 全局变量
HANDLE last_handle = 0;  // 共享内存指针

ULONG64* pulStackTrace;
BreakPointData  g_BreakPointData = { 0 };
BreakPointMessage g_BreakPointMessage = { 0 };
BOOLEAN g_Run = FALSE;
KEVENT g_Event;
KEVENT g_APC_Event;
HANDLE ReopenSharedMemorySection()
{

  
    NTSTATUS status;
    UNICODE_STRING sectionName;
    OBJECT_ATTRIBUTES objAttr;

    // 初始化节区名称
    RtlInitUnicodeString(&sectionName, SHARED_MEMORY_NAME);

    // 初始化对象属性
    InitializeObjectAttributes(&objAttr, &sectionName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    // 打开节区
    HANDLE newSectionHandle;
    status = ZwOpenSection(&newSectionHandle, SECTION_MAP_READ | SECTION_MAP_WRITE, &objAttr);
    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to reopen shared memory section: 0x%x\n", status);
        return 0;
    }

    // 更新全局节区句柄
    return  newSectionHandle;
}


NTSTATUS LockMemoryForDPC(PVOID VirtualAddress, SIZE_T Length)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        // 分配 MDL 结构
        g_Mdl = IoAllocateMdl(
            VirtualAddress,    // 虚拟内存地址
            (ULONG)Length,     // 要锁定的内存长度
            FALSE,             // 不需要关联 MDL 到其他 MDL
            FALSE,             // 没有分配失败时触发例外
            NULL               // 无需传递 IRP
        );

        if (!g_Mdl)
        {
            DbgPrint("Failed to allocate MDL.\n");
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        // 锁定内存到物理内存中
        MmProbeAndLockPages(
            g_Mdl,             // MDL 结构
            KernelMode,        // 调用者上下文（KernelMode 或 UserMode）
            IoReadAccess       // 指定访问类型：读、写或读写
        );

        // 获取内存的系统地址（非分页池地址）
        PVOID LockedMemory = MmGetSystemAddressForMdlSafe(
            g_Mdl,             // MDL 结构
            NormalPagePriority // 分配优先级
        );

        if (!LockedMemory)
        {
            DbgPrint("Failed to map locked memory.\n");
            status = STATUS_INSUFFICIENT_RESOURCES;
            MmUnlockPages(g_Mdl);
            IoFreeMdl(g_Mdl);
            g_Mdl = NULL;
            return status;
        }

        DbgPrint("Memory successfully locked and mapped.\n");

        // 在这里可以使用 LockedMemory 访问锁定的内存。
        // 示例：写入数据
        RtlZeroMemory(LockedMemory, Length);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        DbgPrint("Exception occurred while locking memory.\n");
        status = GetExceptionCode();
    }

    return status;
}

VOID UnlockMemory()
{
    if (g_Mdl)
    {
        // 解锁内存
        MmUnlockPages(g_Mdl);

        // 释放 MDL 结构
        IoFreeMdl(g_Mdl);
        g_Mdl = NULL;

        DbgPrint("Memory successfully unlocked.\n");
    }
}
NTSTATUS startETW(HANDLE Pid)
{

    if (Pid == g_pid)
        return STATUS_SUCCESS;

    NTSTATUS status;
    UNICODE_STRING sectionName;
    OBJECT_ATTRIBUTES objAttr;
    LARGE_INTEGER sectionSize;
    // 创建共享内存段
    if (!g_SharedMemorySection)
    {
        // 设置共享内存大小
        sectionSize.QuadPart = 0x10 * 1024 * 1024;
        // 设置共享内存段的名称
        RtlInitUnicodeString(&sectionName, SHARED_MEMORY_NAME);

        // 初始化对象属性
        InitializeObjectAttributes(&objAttr, &sectionName, OBJ_CASE_INSENSITIVE, NULL, NULL);

        status = ZwCreateSection(
            &g_SharedMemorySection,
            SECTION_MAP_READ | SECTION_MAP_WRITE,
            &objAttr,
            &sectionSize,
            PAGE_READWRITE,
            SEC_COMMIT,
            NULL  // 无需文件，创建匿名共享内存
        );
        if (!NT_SUCCESS(status)) {
            DbgPrint("ZwCreateSection failed: 0x%x\n", status);
            //[todo] 多次调用startETW后 卸载驱动释放不掉节区，目前是直接用之前创建好的，后面要改。
            g_SharedMemorySection = ReopenSharedMemorySection(); //从新获取句柄
        }

        // 映射共享内存段到内核空间

        status = ObReferenceObjectByHandle(g_SharedMemorySection,
            SECTION_ALL_ACCESS,
            NULL,
            KernelMode,
            &g_section_object,
            NULL);
        if (status != STATUS_SUCCESS) {
            DbgPrint("ObReferenceObjectByHandle failed. err=%x\n", status);
        }

        SIZE_T nViewSize = 0;

        status = MmMapViewInSystemSpace(
            g_section_object,       // 节区句柄
            &g_kernelModeAddress,          // 输出：内核地址空间映射地址
            &nViewSize                   // 映射的视图大小
        );

        if (!NT_SUCCESS(status)) {
            DbgPrint("MmMapViewInSystemSpace failed: 0x%x\n", status);
            return status;
        }

        RtlZeroMemory(g_kernelModeAddress, 0x10 * 1024 * 1024);

        LockMemoryForDPC(g_kernelModeAddress, 0x10 * 1024 * 1024);
        g_kernelModeAddress1 = g_kernelModeAddress;
    }

    SIZE_T nViewSize = 0;
    CLIENT_ID clientId;
    clientId.UniqueProcess = (HANDLE)Pid; // 进程 ID
    clientId.UniqueThread = NULL;
    HANDLE p;
    OBJECT_ATTRIBUTES objAttrs;
    //if (last_handle)
    //{
    //    ZwClose(last_handle);
    //}

    InitializeObjectAttributes(&objAttrs, NULL, OBJ_KERNEL_HANDLE, NULL, NULL)
        status = ZwOpenProcess(&p, PROCESS_ALL_ACCESS, &objAttrs, &clientId);

    HANDLE  newSectionHandle = ReopenSharedMemorySection(); //从新获取句柄
    g_SharedMemory = 0;
    last_handle = newSectionHandle;

    status = ZwMapViewOfSection(
        newSectionHandle,                  // 节句柄
        p,
        &g_SharedMemory,                  // 输出映射地址
        0,                                // 保留区地址
        0,                                // 视图大小（由系统决定）
        NULL,                             // 偏移量
        &nViewSize,                        // 返回实际视图大小
        ViewUnmap,                        // 视图分配类型
        0,                                // 映射选项
        PAGE_READWRITE                    // 页属性
    );

    DbgPrint("Shared memory created and mapped in kernel space.\n");

    return status;
}

NTSTATUS Dispatch(PCommPackage pPackage)
{
    ULONG64 cmd = pPackage->ullCmd;
    NTSTATUS st = STATUS_UNSUCCESSFUL;

    switch (cmd)
    {
    case  ASD_TEST:
    {
        st = STATUS_SUCCESS;
    }
    break;
    case  ASD_START_ETW:
    {
        PCommStruct pinfo = (PCommStruct)pPackage->ullInData;

        st = startETW(pinfo->hPID);
        pinfo->ret = (ULONG64)g_SharedMemory;
    }
    break;
    case  ASD_SET_BREAK_POINT:
    {
   
        PBreakPointData pinfo = (PBreakPointData)pPackage->ullInData;
        memcpy(&g_BreakPointData, pinfo, sizeof(BreakPointData));
        KeInitializeEvent(&g_Event, SynchronizationEvent, FALSE);
        st = STATUS_SUCCESS;

    }
    break;
    case  ASD_DELETE_BREAK_POINT:
    {
        KeSetEvent(&g_Event, 0, 0);
        RtlZeroMemory(&g_BreakPointData, sizeof(BreakPointData));
        st = STATUS_SUCCESS;
    }
    break;
    case  ASD_RUN_BREAK_POINT: //这里读取的是 堆栈信息
    {

        ULONG64* pinfo = (PULONG64)pPackage->ullInData;
        LARGE_INTEGER  Timeout;
	    Timeout.QuadPart = -100000;// 10000*100ns
        KeSetEvent(&g_Event, 0, 0);
        KeInitializeEvent(&g_APC_Event, SynchronizationEvent, FALSE);
        KeWaitForSingleObject(&g_APC_Event, WrExecutive, KernelMode, FALSE, &Timeout);

        if(pulStackTrace)
           memcpy(pinfo, pulStackTrace, 0x100);
        st = STATUS_SUCCESS;
    }
    break;
    case  ASD_GET_BREAK_POINT_MESSAGE: //这里读取的是寄存器
    {
    
        PBreakPointMessage pinfo = (PBreakPointMessage)pPackage->ullInData;
        memcpy(pinfo, &g_BreakPointMessage, sizeof(BreakPointData));
        st = STATUS_SUCCESS;
    }
    break;
   /* case  ASD_GET_ALL_MODULE:
    {
        PGET_ALL_MODULE_INFO pinfo = (PGET_ALL_MODULE_INFO)pPackage->ullInData;
        st = ASD_GetAllModule(pinfo->ullPID, pinfo->ullBufferAddress);
    }break;*/
    default:
        break;
    }
    return st;
}
static NTSTATUS DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    NTSTATUS status =0;
    EtwHookManager::get_instance()->destory();
    g_kernelModeAddress1 = 0;
    // 清理资源
    UnlockMemory();
    if (g_kernelModeAddress) {
        status = MmUnmapViewInSystemSpace(g_kernelModeAddress);
        g_kernelModeAddress = NULL;
    }
    if (g_SharedMemory)
    {
        status =    ZwUnmapViewOfSection(ZwCurrentProcess(), g_SharedMemory);
    }
    if (g_SharedMemorySection)
    {
        status =   ZwClose(g_SharedMemorySection);
    }
    if (last_handle)
    {
        status =  ZwClose(last_handle);
    }
    unRegisterComm(DriverObject);
	return STATUS_SUCCESS;
}

EXTERN_C NTSTATUS DriverEntry( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    pulStackTrace = (ULONG64*)ExAllocatePool(NonPagedPool, 0x1000);
    NTSTATUS status = STATUS_SUCCESS;
	UNREFERENCED_PARAMETER( RegistryPath);
	DriverObject->DriverUnload = reinterpret_cast<PDRIVER_UNLOAD>(DriverUnload);
    status = RegisterComm(DriverObject,Dispatch);
    status = EtwHookManager::get_instance()->init();
    return status;
}


