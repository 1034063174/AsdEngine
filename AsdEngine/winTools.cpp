#include "WinTools.h"
#include <tlhelp32.h>
#include <QDebug>

QList<QPair<int, QString>> WinTools::enumerateProcesses()
{
    return enumerateWindowsProcesses();
}

QList<QPair<int, QString>> WinTools::enumerateWindowsProcesses()
{
    QList<QPair<int, QString>> processList;
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return processList;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return processList;
    }

    do {
        QString processName = QString::fromWCharArray(pe32.szExeFile);
        int pid = pe32.th32ProcessID;
        processList.append(qMakePair(pid, processName));
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return processList;
}

bool WinTools::readProcessMemory(int pid, void* address, void* buffer, size_t size)
{
    static  int lastPid = 0;
    static  HANDLE lasthProcess = 0;
    HANDLE hProcess;
    if (pid == lastPid)
    {
        hProcess = lasthProcess;
    }
    else
    {
        CloseHandle(lasthProcess);
        hProcess = OpenProcess(PROCESS_VM_READ, FALSE, pid);
        lasthProcess = hProcess;
        lastPid = pid;
    }


    if (!hProcess) {
        qDebug() << "Failed to open process for reading.";
        return false;
    }

    SIZE_T bytesRead;
    BOOL result = ReadProcessMemory(hProcess, address, buffer, size, &bytesRead);



    if (!result || bytesRead != size) {
     //   qDebug() << "Failed to read process memory.";
        return false;
    }
 
    return true;
}

bool WinTools::writeProcessMemory(int pid, void* address, const void* buffer, size_t size)
{
    HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);
    if (!hProcess) {
        qDebug() << "Failed to open process for writing.";
        return false;
    }

    SIZE_T bytesWritten;
    BOOL result = WriteProcessMemory(hProcess, address, buffer, size, &bytesWritten);

    CloseHandle(hProcess);

    if (!result || bytesWritten != size) {
        qDebug() << "Failed to write process memory.";
        return false;
    }

    return true;
}

bool WinTools::changeMemoryProtection(int pid, void* address, size_t size, DWORD newProtection, DWORD& oldProtection)
{
    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION, FALSE, pid);
    if (!hProcess) {
        qDebug() << "Failed to open process for changing memory protection.";
        return false;
    }

    BOOL result = VirtualProtectEx(hProcess, address, size, newProtection, &oldProtection);

    CloseHandle(hProcess);

    if (!result) {
        qDebug() << "Failed to change memory protection.";
        return false;
    }

    return true;
}

void* WinTools::allocateMemory(int pid, size_t size, DWORD allocationType, DWORD protection)
{
    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);
    if (!hProcess) {
        qDebug() << "Failed to open process for memory allocation.";
        return nullptr;
    }

    void* address = VirtualAllocEx(hProcess, nullptr, size, allocationType, protection);

    CloseHandle(hProcess);

    if (!address) {
        qDebug() << "Failed to allocate memory.";
    }

    return address;
}

HANDLE WinTools::createRemoteThread(int pid, void* startAddress, void* parameter)
{
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);
    if (!hProcess) {
        qDebug() << "Failed to open process for creating remote thread.";
        return nullptr;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)startAddress, parameter, 0, nullptr);
    if (!hThread) {
        qDebug() << "Failed to create remote thread.";
    }

    CloseHandle(hProcess);
    return hThread;
}
int WinTools::getPidByProcessName(const QString& processName)
{
    QList<QPair<int, QString>> processes = enumerateProcesses();
    for (const auto& process : processes) {
        if (process.second == processName) {
            return process.first;
        }
    }
    return -1;
}

ULONG64 WinTools::getModuleBaseAddress(int pid, const QString& moduleName)
{
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(hModuleSnap, &me32)) {
        CloseHandle(hModuleSnap);
        return 0;
    }

    do {
        if (QString::fromWCharArray(me32.szModule) == moduleName) {
            CloseHandle(hModuleSnap);
            return reinterpret_cast<ULONG64>(me32.modBaseAddr);
        }
    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
    return 0;
}