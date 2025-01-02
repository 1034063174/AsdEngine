#include <Windows.h>
#include <stdio.h>
#include <string>
#include <map>
#include <set>
#include"../../asd_ETW/asd_ETW/Utils/NativeEnums.h"
#pragma comment(lib,"../GetSympol/GetSympolDLL.lib")

struct field_info {
    int64_t offset;
    int64_t bitfield_offset;

    static field_info from_map(const std::map<std::string, int64_t>& m) {
        field_info info{};
        info.offset = m.at("offset");
        info.bitfield_offset = m.at("bitfield_offset");
        return info;
    }
};

class IAnimal
{
public:
    virtual  std::map<std::string, int64_t> get_symbol(const std::set<std::string>& names)const = 0;

    virtual std::map<std::string, std::map<std::string, field_info>>get_struct(const std::map<std::string, std::set<std::string>>& names)const = 0;

    virtual  std::map<std::string, std::map<std::string, int64_t>>get_enum(const std::map<std::string, std::set<std::string>>& names)const = 0;
};
typedef NTSTATUS(WINAPI* PFUN_NtQuerySystemInformation)(
    _In_      SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _Inout_   PVOID                    SystemInformation,
    _In_      ULONG                    SystemInformationLength,
    _Out_opt_ PULONG                   ReturnLength
    );
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
typedef struct _RTL_PROCESS_MODULE_INFORMATION_EX
{
    USHORT NextOffset;
    RTL_PROCESS_MODULE_INFORMATION BaseInfo;
    ULONG ImageChecksum;
    ULONG TimeDateStamp;
    PVOID DefaultBase;
} RTL_PROCESS_MODULE_INFORMATION_EX, * PRTL_PROCESS_MODULE_INFORMATION_EX;
struct KernelModuleInfo {

    std::string Name;
    std::string FullPath;
    std::string NtPath;
    HANDLE hSection;
    void* MappedBase;
    void* ImageBase;
    uint32_t ImageSize;
    uint32_t Flags;
    uint16_t LoadOrderIndex;
    uint16_t InitOrderIndex;
    uint16_t LoadCount;
    void* DefaultBase;
    uint32_t ImageChecksum;
    uint32_t TimeDateStamp;
};
ULONG64 RetKernelAddr(const char* szDllname)  //传入内核模块名称，获取其地址。
{
    PFUN_NtQuerySystemInformation pFun = NULL;
    pFun = (PFUN_NtQuerySystemInformation)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQuerySystemInformation");

    PVOID szInfo = (PVOID)malloc(1);
    ULONG uReturnedLEngth = 0;
    NTSTATUS status = pFun(SystemModuleInformationEx, szInfo, sizeof(szInfo), &uReturnedLEngth);
    szInfo = (PVOID)malloc(uReturnedLEngth);
    status = pFun(SystemModuleInformationEx, szInfo, uReturnedLEngth, &uReturnedLEngth);
    //if (status != 0)
    //    return 0;
    PRTL_PROCESS_MODULE_INFORMATION_EX pSystemInformation = (PRTL_PROCESS_MODULE_INFORMATION_EX)szInfo;

    KernelModuleInfo temp;

    while (1)
    {
        temp.ImageBase = pSystemInformation->BaseInfo.ImageBase;
        temp.Name = std::string((PCSTR)((BYTE*)pSystemInformation->BaseInfo.FullPathName + pSystemInformation->BaseInfo.OffsetToFileName));

        if (strcmp(temp.Name.c_str(), szDllname) == 0)
        {
            return   (ULONG64)temp.ImageBase;
        }

        pSystemInformation = (RTL_PROCESS_MODULE_INFORMATION_EX*)((BYTE*)pSystemInformation + pSystemInformation->NextOffset);
        if (pSystemInformation->NextOffset == 0)
            break;
    }
    return 0;

}



extern "C"  IAnimal * CreateMyClass(char* PdbPathname);

//  error  = -1   服务器未下载该pdb 目前正在下载稍后再试
// error = -2     服务器正在下载别的pdb文件，目前正忙，你的下次请求将可能开始下载你所需要的pdb文件即返回-1， （-2 完了之后 必然要经过-1才能正常）
std::map<std::string, int64_t>  GetAllFunction(const char* szDllname)
{
    ULONG64   dllImageBase = 0;
    IAnimal* pdb=0;

     
    try {

        if (strcmp("ntoskrnl.exe", szDllname)==0)
        {
            dllImageBase = RetKernelAddr("ntoskrnl.exe");
            pdb = CreateMyClass((char*)R"(C:\Windows\System32\ntoskrnl.exe)");
        }
        else if (strcmp("win32k.sys", szDllname)==0)
        {
            dllImageBase = RetKernelAddr("win32k.sys");
            pdb = CreateMyClass((char*)R"(C:\Windows\System32\win32k.sys)");
        }
        {//获取符号
            auto offsets = pdb->get_symbol(std::set<std::string>
            {
                "GetAllFunction",
            });
            if (offsets["error"] != 0)
            {
                printf("当前错误为error  %d  ", offsets["error"]);
            }
            else
            {
                for (auto& pair : offsets) {
                    pair.second += dllImageBase; // 给每个 int64_t 加 1
                }
                return offsets;
           
            }
        }
    }
    catch (std::exception& e) {
        printf("%s  \r\n", e.what());
        return (std::map<std::string, int64_t>{ {"error", -3}});
    }
}