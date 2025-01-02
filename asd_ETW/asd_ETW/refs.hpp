#ifndef _REFS_H_

#define _REFS_H_

#include <fltKernel.h>
#include <ntimage.h>
#include <intrin.h>


/*不报警告地申请内核内存 可以替换ExAlloatePoolWithTag*/
template<typename T>
static inline T* kalloc(POOL_TYPE pool_type, SIZE_T size = 0, ULONG tag = 'refs') {

	auto func_name = UNICODE_STRING{};
	auto func = (void*)(nullptr);
	auto ret = (void*)(nullptr);

	RtlInitUnicodeString(&func_name, L"ExAllocatePoolZero");
	func = MmGetSystemRoutineAddress(&func_name);
	if (size == 0) {
		size = sizeof(T);
	}

	if (func) {

		auto f = reinterpret_cast<void* (*)(POOL_TYPE, SIZE_T, ULONG)>(func);

		ret = f(pool_type, size, tag);
	}
	else {
		/*低版本windows*/
		RtlInitUnicodeString(&func_name, L"ExAllocatePoolWithTag");
		func = MmGetSystemRoutineAddress(&func_name);

		auto f = reinterpret_cast<void* (*)(POOL_TYPE, SIZE_T, ULONG)>(func);

		/*如果连这个函数找不到蓝屏是正常的*/
		ret = f(pool_type, size, tag);
	}

	return reinterpret_cast<T*>(ret);
}





#define MAXIMUM_FILENAME_LENGTH 256

typedef struct _SYSTEM_MODULE_ENTRY
{
#ifdef _WIN64
	ULONGLONG Unknown1;
	ULONGLONG Unknown2;
#else
	ULONG Unknown1;
	ULONG Unknown2;
#endif
	PVOID BaseAddress;
	ULONG Size;
	ULONG Flags;
	ULONG EntryIndex;
	USHORT NameLength;  // Length of module name not including the path, this field contains valid value only for NTOSKRNL module
	USHORT PathLength;  // Length of 'directory path' part of modulename
	CHAR Name[MAXIMUM_FILENAME_LENGTH];
} SYSTEM_MODULE_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG Count;
#ifdef _WIN64
	ULONG Unknown1;
#endif
	SYSTEM_MODULE_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION;


extern "C" {

	NTKERNELAPI PVOID RtlPcToFileHeader(PVOID pc, PVOID* base);
};

#endif // _REFS_H_
