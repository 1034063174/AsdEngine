#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "comm.h"

#define SHARED_MEMORY_NAME L"\\BaseNamedObjects\\SharedMemory"  // �����ڴ�����
#define IOCTL_READ_DATA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
ULONG64 g_infoaddr =0;
int main()
{
    printf("pid = %d\r\n", GetCurrentProcessId());
    CommStruct info = { 0 };
    info.hPID = (HANDLE)GetCurrentProcessId();
    DriverComm(ASD_START_ETW, &info, sizeof(info));
    printf("��ַ = %llx\r\n", info.ret);
    g_infoaddr = info.ret;
    system("pause");
    // ���������ڴ�ӳ���ļ�
    const SIZE_T sharedMemSize = 10 * 1024 * 1024; // ���蹲���ڴ��СΪ1024�ֽ�
    ULONG64 MaxSize = ((10 * 1024 * 1024) / (sizeof(SyscallData) + 0x10)) - 1;//0x10 ���ݴ�
    ULONG64 ticktack = 0;
    while (true)
    {
        for (int i = 0; i <= MaxSize; i++)
        {
            PSyscallData  temp = (PSyscallData)((ULONG64)g_infoaddr + i * (sizeof(SyscallData) + 0x10));
            if (temp->ticktack > ticktack)
            {
                ticktack = temp->ticktack;
                printf(" %d  %s %d %d \r\n", temp->ticktack, temp->Name, temp->Pid, temp->ticktack);

            }
            else
            {
                Sleep(1);
                break;
            }

        }

    }
    system("pause");

    return 0;
}
