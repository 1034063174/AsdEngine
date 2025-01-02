#include "ReadEtwMsg.h"
#include"../../comm/comm.h"
#include <QMap>
#include "tools/GetPdb.h"

#define SHARED_MEMORY_NAME L"\\BaseNamedObjects\\SharedMemory"  // �����ڴ�����
#define IOCTL_READ_DATA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
ULONG64 g_infoaddr = 0;

ReadEtwMsg::ReadEtwMsg()
{
    printf("pid = %d\r\n", GetCurrentProcessId());
    CommStruct info = { 0 };
    info.hPID = (HANDLE)GetCurrentProcessId();
    DriverComm(ASD_START_ETW, &info, sizeof(info));
    printf("��ַ = %llx\r\n", info.ret);
    g_infoaddr = info.ret;
    std::map<std::string, int64_t>  NTstringToInt;
    std::map<std::string, int64_t>  win32stringToInt;
    while (   NTstringToInt.size()<=1)
    {
        NTstringToInt = GetAllFunction("ntoskrnl.exe");
        Sleep(1000);
    }
    while (win32stringToInt.size() <= 1)
    {
        win32stringToInt = GetAllFunction("win32k.sys");
        Sleep(1000);
    }
    for (const auto& pair : NTstringToInt) {
        NTintToString[pair.second] = pair.first;
    }
    for (const auto& pair : win32stringToInt) {
           win32intToString[pair.second] = pair.first;
    }

}
ReadEtwMsg::~ReadEtwMsg() {

}
void ReadEtwMsg::run() {
    const SIZE_T sharedMemSize = 10 * 1024 * 1024; // ���蹲���ڴ��СΪ1024�ֽ�
    ULONG64 MaxSize = ((10 * 1024 * 1024) / (sizeof(SyscallData) + 0x10)) - 1;//0x10 ���ݴ�
    ULONG64 ticktack = 0;
    SyscallData   temp = {};

    while (true)
    {
        for (int i = 0; i <= MaxSize; i++)
        {
            PSyscallData  temps = (PSyscallData)((ULONG64)g_infoaddr + i * (sizeof(SyscallData) + 0x10));
            if (temps->ticktack > ticktack)
            {
                memcpy(&temp, temps, sizeof(SyscallData));
                if (ticktack != 0 && (temp.ticktack - ticktack) > 3)
                {
                    PSyscallData  lasttemp = (PSyscallData)((ULONG64)g_infoaddr + (i - 1) * (sizeof(SyscallData) + 0x10));
                    qDebug("© %llx  %llx %llx  %d \r\n", temp.ticktack, lasttemp->ticktack, ticktack, i);

                    std::string FuncName = win32intToString[(temp.OrgSyscall)];
                    if (FuncName.empty())
                    {
                                  FuncName = NTintToString[(temp.OrgSyscall)];
                    }
                    Send({
                            {"ticktack", QString::number(temp.ticktack)},
                            {"Pid", QString::number(temp.Pid)},
                            {"OrgSyscall", QString::asprintf("0x%llX", temp.OrgSyscall)},
                            {"FuncName", QString::fromStdString(FuncName) }
                        });


                    //  return;
                }
                ticktack = temp.ticktack;
                //  qDebug(" %llx  %s %llx \r\n", temp->ticktack, temp->Name, temp->Pid);
            }
            //else
            //{
            //    last_i = i;
            //    qDebug("222 %llx  %llx %d \r\n", temp->ticktack, ticktack, i);
            //    break;
            //}
        }

    }
}
void ReadEtwMsg::Send(QMap<QString, QString> list)
{
    emit SendToTable(list);
}