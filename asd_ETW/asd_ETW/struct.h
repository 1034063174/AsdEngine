#pragma once

typedef struct _CommPackage
{
	ULONG64 ullCmd;
	ULONG64 ullInData;
	ULONG64 ullInlen;
	ULONG64 ullOutData;
	ULONG64 ullOutlen;

}CommPackage, * PCommPackage;


typedef enum _ASD_CMD
{
	ASD_TEST,
	ASD_START_ETW = 1,
	//ASD_GET_MODULE = 1 ,
	//ASD_READ_MEMORY  =2 ,
	//ASD_WRITE_MEMORY = 3,
	//ASD_QUERY_MEMORY = 4,
	//ASD_FORCE_FILE = 5 ,
	//ASD_GET_ALL_MODULE = 6,
	//ASD_GET_ALL_CallBack = 7,
	//ASD_INIT_API_FUNC = 8,
	//ASD_INIT_STRUCT = 9,
	//ASD_CHANGE_HANDLE_GRANTEDACCESS = 10,
	//ASD_USER_PROTECT_PROCESS =11,
	//ASD_KERNEL_PROTECT_PROCESS =12,
	//ASD_ATTACH_READ_MEMORY = 13,

}ASD_CMD;

typedef struct _CommStruct  //²éÑ¯Ä£¿é
{
	HANDLE  hPID;
	ULONG64  ret;

}CommStruct,* PCommStruct;

typedef struct _SyscallData
{
	ULONG64 ticktack;
	ULONG64	Pid;
	ULONG64	OrgSyscall;
	ULONG64 r9 = 0;
	ULONG64 r8 = 0;
	ULONG64 rdx = 0;
	ULONG64 rcx = 0;
	char Name[0x20] = { 0 };

}SyscallData, * PSyscallData;