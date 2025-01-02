#include "comm.h"
#include <stdio.h>
#include<winternl.h>  //ntdllµÄº¯Êý   Èçntcreatefile
#include"../asd_ETW/struct.h"
#include <intrin.h>




HANDLE  HFILEs = CreateFileW(SYM_NAME, FILE_GENERIC_READ | FILE_GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);





BOOLEAN DriverComm(ULONG ulCmd, PVOID pInData, int nLen)
{
	CommPackage pAge;
	pAge.ullCmd = ulCmd;
	pAge.ullInData = (ULONG_PTR)pInData;
	pAge.ullInlen = nLen;
	pAge.ullOutlen = -1;
	if (HFILEs == NULL || HFILEs == INVALID_HANDLE_VALUE)
	{
		printf("Error HFILEs %d \r\n",GetLastError());
	}
	else
	{
		ULONG ulRelen = 0;
		BOOLEAN isSuccess = DeviceIoControl(HFILEs, TEST, &pAge, sizeof(pAge), &pAge, sizeof(pAge), &ulRelen, NULL);
		return 1;
	}
	return 0;
}