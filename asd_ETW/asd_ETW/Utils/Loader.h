#pragma once
#include "PEB.h"
namespace Loader 
{
    namespace Module
    {
        typedef struct _RTL_PROCESS_MODULE_INFORMATION {
            HANDLE Section;                 // Not filled in
            PVOID MappedBase;
            PVOID ImageBase;
            ULONG ImageSize;
            ULONG Flags;
            USHORT LoadOrderIndex;
            USHORT InitOrderIndex;
            USHORT LoadCount;
            USHORT OffsetToFileName;
            UCHAR  FullPathName[256];
        } RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

        typedef struct _RTL_PROCESS_MODULES {
            ULONG NumberOfModules;
            RTL_PROCESS_MODULE_INFORMATION Modules[1];
        } RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;

        CHAR* CharToUper(CHAR* wstr, BOOLEAN isAllocateMemory);

        ULONG_PTR QuerySysModule(char* MoudleName, _Out_opt_ ULONG_PTR* module);

        PPEB32 GetProcessPeb32(PEPROCESS Process);

        PPEB GetProcessPeb(PEPROCESS Process);

        HANDLE GetModuleHandle(IN PUNICODE_STRING ModuleName, OUT PULONG ModuleSize = nullptr);

        HANDLE GetModuleHandleByProcessId(IN HANDLE ProcessId, IN PUNICODE_STRING ModuleName, OUT PULONG ModuleSize = nullptr);

        HANDLE GetModuleHandleByProcess(IN PEPROCESS Process, IN PUNICODE_STRING ModuleName, OUT PULONG ModuleSize = nullptr);

        /// <summary>
        /// Get exported function address
        /// </summary>
        /// <param name="pBase">Module base</param>
        /// <param name="name_ord">Function name or ordinal</param>
        /// <param name="pProcess">Target process for user module</param>
        /// <param name="baseName">Dll name for api schema</param>
        /// <returns>Found address, NULL if not found</returns>
        PVOID GetModuleExport(IN PVOID pBase, IN PCCHAR name_ord, IN PEPROCESS pProcess, IN PUNICODE_STRING baseName = nullptr);
    }
}
