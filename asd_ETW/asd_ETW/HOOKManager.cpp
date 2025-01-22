#include "etwhook_manager.hpp"
#include <intrin.h>
#include "ksystem_info.hpp"
#include"Utils/MacroHelper.h"
#include"Utils/Loader.h"
#include"Utils/MemoryUtils.h"
#include "struct.h"
EtwHookManager* EtwHookManager::__instance;

void(*EtwHookManager::__orghalcollectpmccounters)(void*, unsigned long long);

extern BreakPointData  g_BreakPointData ;
extern BreakPointMessage  g_BreakPointMessage;
extern    KEVENT     g_APC_Event;

EtwHookManager* EtwHookManager::get_instance()
{
	if (!__instance) __instance = new EtwHookManager;
	return __instance;
}

NTSTATUS EtwHookManager::init()
{
	auto status = STATUS_UNSUCCESSFUL;

	/*检查是否分配单例的内存了*/
	if (!__instance) return STATUS_MEMORY_NOT_ALLOCATED;

	/*这种方法不支持win7*/
	auto info_instance = kstd::SysInfoManager::getInstance();
	if (info_instance == nullptr) return STATUS_INSUFFICIENT_RESOURCES;
	if (info_instance->getBuildNumber() <= 7601)
	{
		DBG_P("current os version is not supported!\r\n");
		return STATUS_NOT_SUPPORTED;
	}


	do {
		status = this->__initilizer.start_syscall_trace();
		if (!NT_SUCCESS(status)) break;

		/*set value above 1*/

		status = this->__initilizer.open_pmc_counter();
		if (!NT_SUCCESS(status)) break;


		if (this->__initilizer.HalPrivateDispatchTable == nullptr) {
			status = STATUS_UNSUCCESSFUL;
			DBG_P("failed to get HalPrivateDispatchTable address!\r\n");
			break;
		}

		_disable();
		/*swap*/
		__orghalcollectpmccounters = reinterpret_cast<void(*)(void*, unsigned long long)> \
			(this->__initilizer.HalPrivateDispatchTable[__halcollectpmccounters_idx]);

		this->__initilizer.HalPrivateDispatchTable[__halcollectpmccounters_idx] = \
			reinterpret_cast<ULONG_PTR>(hk_halcollectpmccounters);

		_enable();


	} while (false);


	//clean up

	//if fail

	//if suc

	return status;
}

void EtwHookManager::hk_halcollectpmccounters(void* ctx, unsigned long long trace_buffer_end)
{
	UNREFERENCED_PARAMETER(ctx);
	UNREFERENCED_PARAMETER(trace_buffer_end);
	if (KeGetCurrentIrql() <= DISPATCH_LEVEL)
		EtwHookManager::get_instance()->stack_trace_to_syscall();


	return (void)0;
}
EtwHookManager::EtwHookManager() {

	SIZE_T moduleBase = 0;
	ULONG64 size = Loader::Module::QuerySysModule((char*)"ntoskrnl.exe", &moduleBase);

	__nt_img = (void*)moduleBase;

	__KiSystemServiceRepeat =(ULONG_PTR) (Pattens::SearchCodeFromAddress(moduleBase, (ULONG)size, ".text", reca<UCHAR*>((char*)"4c8d15????4c8d1d????f743"), MAXULONG64, MAXULONG64));



	//__KiSystemServiceRepeat = ntos.patternFindSections((unsigned long long)__nt_img, \
	//	"\x4c\x8d\x15\x00\x00\x00\x00\x4c\x8d\x1d\x00\x00\x00\x00\xf7\x43", \
	//	"xxx????xxx????xx", ".text");
// 
	//__nt_img = find_module_base(L"ntoskrnl.exe", &__nt_size);

	//kstd::ParsePE ntos(__nt_img, __nt_size);


	///*注意，这个方法并不严谨！没有直接readmsr IA32_LSTAR 然后使用反汇编引擎解析严谨*/
	////KiSystemServiceRepeat:
	////	4C 8D 15 85 6F 9F 00          lea     r10, KeServiceDescriptorTable
	////	4C 8D 1D FE 20 8F 00          lea     r11, KeServiceDescriptorTableShadow
	////	F7 43 78 80 00 00 00          test    dword ptr[rbx + 78h], 80h; GuiThread
	///*KiSystemServiceRepeat一定位于KiSystemCall64之中，这个直接进行特征码搜索*/

	//__KiSystemServiceRepeat = ntos.patternFindSections((unsigned long long)__nt_img, \
	//	"\x4c\x8d\x15\x00\x00\x00\x00\x4c\x8d\x1d\x00\x00\x00\x00\xf7\x43", \
	//	"xxx????xxx????xx", ".text");

	///*初始化二叉树*/
	//__hookmaps.init();
}

EtwHookManager::~EtwHookManager()
{
	/*关闭etw trace*/
	__initilizer.end_syscall_trace();

	/*恢复HalPrivateHook*/
	_disable();
	this->__initilizer.HalPrivateDispatchTable[__halcollectpmccounters_idx] = \
		reinterpret_cast<ULONG_PTR>(__orghalcollectpmccounters);
	_enable();



}

void EtwHookManager::stack_trace_to_syscall()
{

	if (ExGetPreviousMode() == KernelMode)
	{
		return;
	}



	auto stack_max = (PVOID*)__readgsqword(0x1A8);

	auto cur_stack = (PVOID*)_AddressOfReturnAddress();
	constexpr auto magic1 = 0x501802ul;
	constexpr auto magic2 = 0xf33ul;

	do {

		if (!__KiSystemServiceRepeat) {
			DBG_P("failed to find KiSystemServiceRepeat\r\n");
			break;
		}

		if (!__nt_img) {
			DBG_P("failed to find ntoskrnl.exe");
			break;
		}

		//__debugbreak();

		/*
		*
		*			max
					...
					...
		cur_stack->	xxx
					...
					magic_number
					...
					syscall   <-先从上面开始遍历
		*/

		/*开始遍历堆栈*/

		for (; cur_stack < stack_max; cur_stack++) {

			auto stack_as_ushort = reinterpret_cast<PUSHORT>(cur_stack);

			if (*stack_as_ushort != magic2) continue;

			cur_stack++;

			auto stack_as_ulong = reinterpret_cast<PULONG>(cur_stack);

			if (*stack_as_ulong != magic1) continue;

			/*开始遍历*/
			for (; cur_stack < stack_max; cur_stack++) {

				if ((ULONG_PTR)*cur_stack >= (ULONG_PTR)PAGE_ALIGN(__KiSystemServiceRepeat) \
					&&
					(ULONG_PTR)*cur_stack <= (ULONG_PTR)PAGE_ALIGN(__KiSystemServiceRepeat + PAGE_SIZE * 2)
					) {
					//find 注意!!! 这个cur_stck不能100%保证是syscall，因为sys_exit的时候也会走到这
					record_syscall(cur_stack);

					break;
				}

			}

			break;

		}


	} while (false);

}

EXTERN_C PCHAR PsGetProcessImageFileName(PEPROCESS Process);


extern "C"
{
	ULONG64 OrgSyscall;
	void* CommShellcode();
	void CommFunction(ULONG64 arg1, ULONG64 arg2, ULONG64 arg3, ULONG64 arg4);
}

void CommFunction(ULONG64 arg1, ULONG64 arg2, ULONG64 arg3, ULONG64 arg4)
{
	DBG_P("func: %llx    arg: %llx %llx  %llx  %llx\r\n", OrgSyscall, arg1, arg2, arg3, arg4);

}
/*
.text:0000000140412AC1 48 83 EC 50                                                       sub     rsp, 50h
.text : 0000000140412AC5 48 89 4C 24 20                                                  mov[rsp + 20h], rcx
.text : 0000000140412ACA 48 89 54 24 28                                                  mov[rsp + 28h], rdx
.text : 0000000140412ACF 4C 89 44 24 30                                                  mov[rsp + 30h], r8
.text : 0000000140412AD4 4C 89 4C 24 38                                                  mov[rsp + 38h], r9
.text : 0000000140412AD9 4C 89 54 24 40                                                  mov[rsp + 40h], r10
.text : 0000000140412ADE 49 8B CA                                                        mov     rcx, r10
.text : 0000000140412AE1 E8 5A 8B 19 00                                                  call    PerfInfoLogSysCallEntry
.text : 0000000140412AE6 48 8B 4C 24 20                                                  mov     rcx, [rsp + 20h]
.text : 0000000140412AEB 48 8B 54 24 28                                                  mov     rdx, [rsp + 28h]
.text : 0000000140412AF0 4C 8B 44 24 30                                                  mov     r8, [rsp + 30h]
.text : 0000000140412AF5 4C 8B 4C 24 38                                                  mov     r9, [rsp + 38h]
.text : 0000000140412AFA 4C 8B 54 24 40                                                  mov     r10, [rsp + 40h]
.text : 0000000140412AFF 48 83 C4 50                                                     add     rsp, 50h
.text : 0000000140412B03 49 8B C2                                                        mov     rax, r10
.text : 0000000140412B06 FF D0                                                           call    rax   //这是call系统函数
*/
extern PVOID g_kernelModeAddress1;
ULONG64 MaxSize = ((10 * 1024 * 1024) / (sizeof(SyscallData) + 0x10)) - 1;//0x10 是容错
extern  KEVENT g_Event;
extern ULONG64* pulStackTrace;
// KAPC environment enumeration
typedef enum _KAPC_ENVIRONMENT {
	OriginalApcEnvironment,
	AttachedApcEnvironment,
	CurrentApcEnvironment,
	InsertApcEnvironment
} KAPC_ENVIRONMENT;

// APC routine function prototypes
typedef VOID(*PKNORMAL_ROUTINE) (
	IN PVOID pNormalContext,
	IN PVOID pSystemArgument1,
	IN PVOID pSystemArgument2
	);

typedef VOID(*PKKERNEL_ROUTINE) (
	IN struct _KAPC* pApc,
	IN OUT PKNORMAL_ROUTINE* pNormalRoutine,
	IN OUT PVOID* pNormalContext,
	IN OUT PVOID* pSystemArgument1,
	IN OUT PVOID* pSystemArgument2
	);

typedef VOID(*PKRUNDOWN_ROUTINE) (
	IN struct _KAPC* pApc
	);

// Function prototypes for APC initialization and insertion
 EXTERN_C VOID KeInitializeApc(
	__out PRKAPC pApc,
	__in PRKTHREAD pThread,
	__in KAPC_ENVIRONMENT eEnvironment,
	__in PKKERNEL_ROUTINE pKernelRoutine,
	__in_opt PKRUNDOWN_ROUTINE pRundownRoutine,
	__in_opt PKNORMAL_ROUTINE pNormalRoutine,
	__in_opt KPROCESSOR_MODE eApcMode,
	__in_opt PVOID pNormalContext
);

 EXTERN_C BOOLEAN KeInsertQueueApc(
	__inout PRKAPC pApc,
	__in_opt PVOID pSystemArgument1,
	__in_opt PVOID pSystemArgument2,
	__in KPRIORITY nIncrement
);


VOID NormalRoutineFunc(
	IN struct _KAPC* pApc,
	IN OUT PKNORMAL_ROUTINE* pNormalRoutine,
	IN OUT PVOID* pNormalContext,
	IN OUT PVOID* pSystemArgument1,
	IN OUT PVOID* pSystemArgument2
)
{
	UNREFERENCED_PARAMETER(pNormalContext);
	UNREFERENCED_PARAMETER(pSystemArgument2);
	UNREFERENCED_PARAMETER(pNormalRoutine);
	UNREFERENCED_PARAMETER(pApc);

	PKEVENT overevent = (PKEVENT)*pSystemArgument1;
	ULONG	ulCapturedFrames = RtlWalkFrameChain((PVOID*)pulStackTrace, 20, UserMode);
	UNREFERENCED_PARAMETER(ulCapturedFrames);
	KeSetEvent(overevent, 0, 0);
;

}
void EtwHookManager::record_syscall(void** call_routine)
{
	//[todo]多核心情况下 存在多个核心同时调用一个api 会漏包，
	static  ULONG64 ticktack = 1;

	if (g_kernelModeAddress1)
	{
		void** SystemCallFunction = &(call_routine[9]);
		//PKTHREAD CurrentThread = (PKTHREAD)__readgsqword(0x188);
		//unsigned int SystemCallIndex = *(unsigned int*)((uintptr_t)CurrentThread + 0x80);
		PCHAR ProcessName = PsGetProcessImageFileName(PsGetCurrentProcess());
		static int NowWrite = 0;
		OrgSyscall = (ULONG64)*SystemCallFunction;
		//ULONG64 r9 = *((ULONG64*)SystemCallFunction - 1);
		//ULONG64 r8 = *((ULONG64*)SystemCallFunction - 2);
		//ULONG64 rdx = *((ULONG64*)SystemCallFunction - 3);
		//ULONG64 rcx = *((ULONG64*)SystemCallFunction - 4);

		if (*(PULONG64)g_kernelModeAddress1 == 0) //如果开头都是0 肯定是让R3清除了
		{
			NowWrite = 0;
		}
		else if (NowWrite >= MaxSize)  //超出了 清除。
		{
		//	RtlZeroMemory(g_kernelModeAddress1, 10 * 1024 * 1024);
			NowWrite = 0;
		}
		else
		{
			NowWrite++;
		}

		PSyscallData  temp = (PSyscallData)((ULONG64)g_kernelModeAddress1 + NowWrite * (sizeof(SyscallData) + 0x10));

		temp->OrgSyscall = OrgSyscall;
		temp->r9 = *((ULONG64*)SystemCallFunction - 1);
		temp->r8 = *((ULONG64*)SystemCallFunction - 2);
		temp->rdx = *((ULONG64*)SystemCallFunction - 3);
		temp->rcx = *((ULONG64*)SystemCallFunction - 4);
		RtlZeroMemory(temp->Name, 0x20);
		memcpy(temp->Name, ProcessName, strlen(ProcessName));
		temp->Pid = (ULONG64)PsGetCurrentProcessId();
		temp->ticktack = ticktack++;       //写id记得放最后，避免先写id直接被R3读取了；
//ticktack放这里能减缓漏包，但是还会漏
		UNREFERENCED_PARAMETER(call_routine);
		do
		{
			if ((g_BreakPointData.Pid == temp->Pid) &&
				(g_BreakPointData.Address == temp->OrgSyscall))
			{
				if (g_BreakPointData.UseR8)
					if (temp->r8 != g_BreakPointData.R8)
						break;
				if (g_BreakPointData.UseR9)
					if (temp->r9 != g_BreakPointData.R9)
						break;
				if (g_BreakPointData.UseRCX)
					if (temp->rcx != g_BreakPointData.RCX)
						break;
				if (g_BreakPointData.UseRDX)
					if (temp->rdx != g_BreakPointData.RDX)
						break;
				g_BreakPointMessage.R8 = temp->r8;
				g_BreakPointMessage.R9 = temp->r9;
				g_BreakPointMessage.RCX = temp->rcx;
				g_BreakPointMessage.RDX = temp->rdx;
				if (MmIsAddressValid((PVOID)temp->r8))
					g_BreakPointMessage.R8DATA =  *(ULONG64*)temp->r8;
				if (MmIsAddressValid((PVOID)temp->r9))
					g_BreakPointMessage.R9DATA = *(ULONG64*)temp->r9;
				if (MmIsAddressValid((PVOID)temp->rcx))
					g_BreakPointMessage.RCXDATA = *(ULONG64*)temp->rcx;
				if (MmIsAddressValid((PVOID)temp->rdx))
					g_BreakPointMessage.RDXDATA = *(ULONG64*)temp->rdx;
	
				
				//irql>=2时无法调用RtlWalkFrameChain
				//所以只能用apc调用，但是当irql=2时又不能执行应用层apc
				//目前的方式时先在irql=2时设置apc，当进程执行完毕后回到应用层时获取调用堆栈。
				//那么目前我们就能获取到 

				static PKAPC pApcusers = NULL;
				if (pApcusers == NULL)
					pApcusers = (PKAPC)ExAllocatePool(NonPagedPool, sizeof(KAPC)*10);
				static ULONG64 tack = 0;
				tack++;
				PKAPC pApcuser = &pApcusers[tack % 10];
				KeInitializeApc(pApcuser, KeGetCurrentThread(), OriginalApcEnvironment, NormalRoutineFunc, NULL, NULL, UserMode, (PVOID)NULL);
				KeInsertQueueApc(pApcuser, &g_APC_Event, 0, 2);
;

				KeWaitForSingleObject(&g_Event, WrExecutive, 0, 0, 0);
			}
		} while (0);

		
	
		//auto hk_map = __hookmaps.find({ call_routine[9],nullptr });

		//if (!hk_map) return;

		//if (hk_map->detour_func) {

		//	call_routine[9] = hk_map->detour_func;
		//}
	}
}
NTSTATUS EtwHookManager::destory()
{
	auto status = STATUS_UNSUCCESSFUL;

	if (!__instance) return STATUS_MEMORY_NOT_ALLOCATED;

	do {

		delete __instance;

		__instance = nullptr;

		status = STATUS_SUCCESS;

	} while (false);


	LARGE_INTEGER delay_time = {};
	delay_time.QuadPart = -10 * 1000000 * 2;
	KeDelayExecutionThread(KernelMode, false, &delay_time);


	return status;
}
