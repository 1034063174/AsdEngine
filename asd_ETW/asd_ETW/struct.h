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
	ASD_SET_BREAK_POINT = 2,
	ASD_DELETE_BREAK_POINT = 3,
	ASD_RUN_BREAK_POINT = 4,
	ASD_GET_BREAK_POINT_MESSAGE = 5,
	ASD_GET_ALL_MODULE = 6,
}ASD_CMD;

typedef struct _CommStruct  
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

typedef struct _BreakPointData
{
	ULONG64	Pid;
	ULONG64	Address;
	ULONG64 R9;
	ULONG64 R8;
	ULONG64 RDX;
	ULONG64 RCX;
	BOOLEAN UseR8;
	BOOLEAN UseR9;
	BOOLEAN UseRDX;
	BOOLEAN UseRCX;
}BreakPointData, * PBreakPointData;

typedef struct _BreakPointMessage
{
	ULONG64 R9;
	ULONG64 R8;
	ULONG64 RDX;
	ULONG64 RCX;
	ULONG64 R9DATA;
	ULONG64 R8DATA;
	ULONG64 RDXDATA;
	ULONG64 RCXDATA;
}BreakPointMessage, * PBreakPointMessage;

//typedef struct _GET_ALL_MODULE_INFO   //查询内存信息
//{
//	HANDLE  ullPID;
//	ULONG64  ullBufferAddress;
//}GET_ALL_MODULE_INFO, * PGET_ALL_MODULE_INFO;
