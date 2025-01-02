#pragma once
#include<Windows.h>
#include"../../asd_ETW/asd_ETW/struct.h"



#define SYM_NAME L"\\\\.\\asd"  //·ûºÅÁ´½Ó  (\\.\rabbit)
#define CODE_CTR_INDEX 0X800 
#define TEST CTL_CODE(FILE_DEVICE_UNKNOWN,CODE_CTR_INDEX,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define TEST1 CTL_CODE(FILE_DEVICE_UNKNOWN,CODE_CTR_INDEX+1,METHOD_BUFFERED,FILE_ANY_ACCESS)

BOOLEAN DriverComm(ULONG cmd, PVOID inData, int len);