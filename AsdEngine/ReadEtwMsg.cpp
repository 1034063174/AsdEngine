#include "ReadEtwMsg.h"
#include"../../comm/comm.h"
#include <QMap>
#include "tools/GetPdb.h"
#include <QVector>
#define SHARED_MEMORY_NAME L"\\BaseNamedObjects\\SharedMemory"  // 共享内存名称
#define IOCTL_READ_DATA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
ULONG64 g_infoaddr = 0;

ReadEtwMsg::ReadEtwMsg(QVector<int>  &includePid, QVector<int> & ExcludePid, QVector<QString>  &ProcessName, QVector<QString> &includeAPI ,int choose , threadConfig config, std::map<std::string, int64_t>  g_NTstringToInt, std::map<std::string, int64_t>  g_win32stringToInt)
{
    m_includePid =includePid;
    m_excludePid=ExcludePid;
    m_processName=ProcessName;
    m_includeAPI=includeAPI;
    chooseModule = choose;

     m_show_ssdt = config.show_ssdt;
     m_show_sssdt = config.show_sssdt;
     m_show_stack = config.show_stack;
     m_show_Registers_Data = config.show_Registers_Data;


    printf("pid = %d\r\n", GetCurrentProcessId());
    CommStruct info = { 0 };
    info.hPID = (HANDLE)GetCurrentProcessId();
    DriverComm(ASD_START_ETW, &info, sizeof(info));
    printf("地址 = %llx\r\n", info.ret);
    g_infoaddr = info.ret;
    std::map<std::string, int64_t>  NTstringToInt = g_NTstringToInt ;
    std::map<std::string, int64_t>  win32stringToInt = g_win32stringToInt;
    
    for (const auto& pair : NTstringToInt) {
        NTintToString[pair.second] = pair.first;
    }
    for (const auto& pair : win32stringToInt) {
        win32intToString[pair.second] = pair.first;
    }
    AGAIN = TRUE;
}
ReadEtwMsg::~ReadEtwMsg() {

}
void ReadEtwMsg::run() {
    const SIZE_T sharedMemSize = 10 * 1024 * 1024; // 假设共享内存大小为1024字节
    ULONG64 MaxSize = ((10 * 1024 * 1024) / (sizeof(SyscallData) + 0x10)) - 1;//0x10 是容错
    ULONG64 ticktack = 0;
    SyscallData   temp = {};

    while (AGAIN)
    {
        for (int i = 0; i <= MaxSize; i++)
        {
            PSyscallData  temps = (PSyscallData)((ULONG64)g_infoaddr + i * (sizeof(SyscallData) + 0x10));
            if (temps->ticktack > ticktack)
            {
                memcpy(&temp, temps, sizeof(SyscallData));
                bool funcissssdt = TRUE; //这里直接默认是sssdt  且api不是ssdt 就是sssdt
                BOOLEAN isprocess = FALSE; //判断是否要打印的进程

                std::string FuncName = win32intToString[(temp.OrgSyscall)];//[todo]过滤一下，太大了
                if (FuncName.empty())
                {
                    funcissssdt = false;
                    FuncName = NTintToString[(temp.OrgSyscall)];
                }


                if (m_show_ssdt == false && funcissssdt == false)
                {
                    goto end;
                }
                if (m_show_sssdt == false && funcissssdt == true)
                {
                    goto end;
                }


                if (chooseModule == 1) //包含
                {
                    if (m_includePid.isEmpty() || m_includePid.contains(temp.Pid))
                    {
                        isprocess = TRUE;
                    }
                }
                else if (chooseModule == 2) //不包含
                {
                    isprocess = TRUE;
                    if (m_excludePid.contains(temp.Pid))
                    {
                        isprocess = FALSE;
                    }
                }
                else if (chooseModule == 3) //不包含
                {
                    if (m_processName.contains(QString::fromStdString(temp.Name)))
                    {
                        isprocess = TRUE;
                    }
                }
                if (isprocess)
                {
                    if ((m_includeAPI[0] == "ALL" || m_includeAPI.isEmpty()) ||
                        (m_includeAPI[0] == "INCLUDE" && m_includeAPI.contains(QString::fromUtf8(FuncName.c_str()))) ||
                        (m_includeAPI[0] == "EXCLUDE" && !m_includeAPI.contains(QString::fromUtf8(FuncName.c_str()))))
                    {
                        Send({
                        {"ticktack", QString::number(temp.ticktack)},
                        {"Pid", QString::number(temp.Pid)},
                        {"OrgSyscall", QString::asprintf("0x%llX", temp.OrgSyscall)},
                        {"FuncName", QString::fromStdString(FuncName) },
                        {"ProcessName", QString::fromStdString(temp.Name) },
                        {"r8",QString::asprintf("0x%llX", temp.r8)},
                        {"r9", QString::asprintf("0x%llX", temp.r9)},
                        {"rcx", QString::asprintf("0x%llX", temp.rcx)},
                        {"rdx",QString::asprintf("0x%llX", temp.rdx)},
                            });
                    }

                }
            end:
                ticktack = temp.ticktack;
            }

        }

    }
}
void ReadEtwMsg::Send(QMap<QString, QString> list)
{
    emit SendToTable(list);
}