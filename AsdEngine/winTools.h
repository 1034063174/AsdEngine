#ifndef WINTOOLS_H
#define WINTOOLS_H

#include <QStringList>
#include <QPair>
#include <windows.h> // 确保包含Windows头文件

class WinTools
{
public:
    static QList<QPair<int, QString>> enumerateProcesses();
    static bool readProcessMemory(int pid, void* address, void* buffer, size_t size);
    static bool writeProcessMemory(int pid, void* address, const void* buffer, size_t size);
    static bool changeMemoryProtection(int pid, void* address, size_t size, DWORD newProtection, DWORD& oldProtection);
    static void* allocateMemory(int pid, size_t size, DWORD allocationType = MEM_COMMIT | MEM_RESERVE, DWORD protection = PAGE_READWRITE);
    static HANDLE createRemoteThread(int pid, void* startAddress, void* parameter);
    static int getPidByProcessName(const QString& processName);
    static ULONG64 getModuleBaseAddress(int pid, const QString& moduleName);

private:
    static QList<QPair<int, QString>> enumerateWindowsProcesses();
};

#endif // WINTOOLS_H
