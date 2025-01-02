

#include<Windows.h>
#include <capstone/capstone.h>
#include <string>
#include <string>
extern "C"
{
#include "XEDParse.h"
#pragma comment(lib, "XEDParse/XEDParse_x64.lib")

//
//#include "asmjit/asmjit.h"
//#pragma comment(lib, "asmjit/asmjit.lib")

}


class AsmEngine
{

public:
    AsmEngine(cs_mode mode);
    ~AsmEngine();

public:
    std::string  Asm2Hex(std::string AsmCode, DWORD64 Addr);
    void Hex2Asm(unsigned char* CODE, size_t size, DWORD64 Addr);
    cs_err error;
    XEDPARSE xed = { 0 };
    std::string ASm2HexCode = "";
private:
    csh handle; // 生成调用capstone API的句柄
    cs_insn* insn; // 存储指令的详细信息
    size_t count;
};