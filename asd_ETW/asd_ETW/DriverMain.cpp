#include <ntifs.h>
#include "Utils/CppSupport.h"
#include "etwhook_init.hpp"
#include "etwhook_manager.hpp"
#include"Utils/MacroHelper.h"
#include <wdm.h>
#include"comm/IoComm.h"

#define SHARED_MEMORY_NAME L"\\BaseNamedObjects\\SharedMemory"  // �����ڴ�����

void* g_section_object;
HANDLE g_pid = 0;  // �����ڴ�ָ��
PVOID g_SharedMemory = NULL;  // �����ڴ�ָ��
HANDLE g_SharedMemorySection = NULL;  // �����ڴ�ξ��
HANDLE g_kernelModeAddress1 = NULL;  // 
PVOID g_kernelModeAddress = NULL;  // �ں��ڴ�ָ��
PMDL g_Mdl = NULL; // MDL ȫ�ֱ���
HANDLE last_handle = 0;  // �����ڴ�ָ��

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

    // ��ʼ����������
    RtlInitUnicodeString(&sectionName, SHARED_MEMORY_NAME);

    // ��ʼ����������
    InitializeObjectAttributes(&objAttr, &sectionName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    // �򿪽���
    HANDLE newSectionHandle;
    status = ZwOpenSection(&newSectionHandle, SECTION_MAP_READ | SECTION_MAP_WRITE, &objAttr);
    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to reopen shared memory section: 0x%x\n", status);
        return 0;
    }

    // ����ȫ�ֽ������
    return  newSectionHandle;
}


NTSTATUS LockMemoryForDPC(PVOID VirtualAddress, SIZE_T Length)
{
    NTSTATUS status = STATUS_SUCCESS;

    __try
    {
        // ���� MDL �ṹ
        g_Mdl = IoAllocateMdl(
            VirtualAddress,    // �����ڴ��ַ
            (ULONG)Length,     // Ҫ�������ڴ泤��
            FALSE,             // ����Ҫ���� MDL ������ MDL
            FALSE,             // û�з���ʧ��ʱ��������
            NULL               // ���贫�� IRP
        );

        if (!g_Mdl)
        {
            DbgPrint("Failed to allocate MDL.\n");
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        // �����ڴ浽�����ڴ���
        MmProbeAndLockPages(
            g_Mdl,             // MDL �ṹ
            KernelMode,        // �����������ģ�KernelMode �� UserMode��
            IoReadAccess       // ָ���������ͣ�����д���д
        );

        // ��ȡ�ڴ��ϵͳ��ַ���Ƿ�ҳ�ص�ַ��
        PVOID LockedMemory = MmGetSystemAddressForMdlSafe(
            g_Mdl,             // MDL �ṹ
            NormalPagePriority // �������ȼ�
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

        // ���������ʹ�� LockedMemory �����������ڴ档
        // ʾ����д������
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
        // �����ڴ�
        MmUnlockPages(g_Mdl);

        // �ͷ� MDL �ṹ
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
    // ���������ڴ��
    if (!g_SharedMemorySection)
    {
        // ���ù����ڴ��С
        sectionSize.QuadPart = 0x10 * 1024 * 1024;
        // ���ù����ڴ�ε�����
        RtlInitUnicodeString(&sectionName, SHARED_MEMORY_NAME);

        // ��ʼ����������
        InitializeObjectAttributes(&objAttr, &sectionName, OBJ_CASE_INSENSITIVE, NULL, NULL);

        status = ZwCreateSection(
            &g_SharedMemorySection,
            SECTION_MAP_READ | SECTION_MAP_WRITE,
            &objAttr,
            &sectionSize,
            PAGE_READWRITE,
            SEC_COMMIT,
            NULL  // �����ļ����������������ڴ�
        );
        if (!NT_SUCCESS(status)) {
            DbgPrint("ZwCreateSection failed: 0x%x\n", status);
            //[todo] ��ε���startETW�� ж�������ͷŲ���������Ŀǰ��ֱ����֮ǰ�����õģ�����Ҫ�ġ�
            g_SharedMemorySection = ReopenSharedMemorySection(); //���»�ȡ���
        }

        // ӳ�乲���ڴ�ε��ں˿ռ�

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
            g_section_object,       // �������
            &g_kernelModeAddress,          // ������ں˵�ַ�ռ�ӳ���ַ
            &nViewSize                   // ӳ�����ͼ��С
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
    clientId.UniqueProcess = (HANDLE)Pid; // ���� ID
    clientId.UniqueThread = NULL;
    HANDLE p;
    OBJECT_ATTRIBUTES objAttrs;
    //if (last_handle)
    //{
    //    ZwClose(last_handle);
    //}

    InitializeObjectAttributes(&objAttrs, NULL, OBJ_KERNEL_HANDLE, NULL, NULL)
        status = ZwOpenProcess(&p, PROCESS_ALL_ACCESS, &objAttrs, &clientId);

    HANDLE  newSectionHandle = ReopenSharedMemorySection(); //���»�ȡ���
    g_SharedMemory = 0;
    last_handle = newSectionHandle;

    status = ZwMapViewOfSection(
        newSectionHandle,                  // �ھ��
        p,
        &g_SharedMemory,                  // ���ӳ���ַ
        0,                                // ��������ַ
        0,                                // ��ͼ��С����ϵͳ������
        NULL,                             // ƫ����
        &nViewSize,                        // ����ʵ����ͼ��С
        ViewUnmap,                        // ��ͼ��������
        0,                                // ӳ��ѡ��
        PAGE_READWRITE                    // ҳ����
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
    case  ASD_RUN_BREAK_POINT: //�����ȡ���� ��ջ��Ϣ
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
    case  ASD_GET_BREAK_POINT_MESSAGE: //�����ȡ���ǼĴ���
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
    // ������Դ
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


