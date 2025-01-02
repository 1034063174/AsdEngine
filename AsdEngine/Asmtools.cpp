#include<Windows.h>
#include"asmtools.h"
#include<stdio.h>
#include"LogTools.h"
#include"luaClass.h"
#include <QDebug>
#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdio>
#include < iomanip >


extern "C"
{
#include "XEDParse.h"
#pragma comment(lib, "XEDParse/XEDParse_x64.lib")
}

//MyThread thread;
//thread.start(); // 启动线程
// 使用全局日志记录器记录消息

#pragma comment(lib,"capstone.lib")
std::vector<std::string> splitStringByNewline(const std::string& str) {
    std::vector<std::string> result;
    std::istringstream stream(str);
    std::string line;
    while (std::getline(stream, line)) {
        result.push_back(line);
    }
    return result;
}

AsmEngine::AsmEngine(cs_mode Mode)
{
    csh handle; // 生成调用capstone API的句柄
    cs_insn* insn; // 存储指令的详细信息
    size_t count;
    if (Mode == CS_MODE_64)
        xed.x64 = TRUE;
    else
        xed.x64 = false;


    error = cs_open(CS_ARCH_X86, Mode, &handle);
    if (error ) {
        printf("ERROR: Failed to initialize engine!\n");
    }
}
AsmEngine::~AsmEngine()
{
    cs_free(insn, count);
    cs_close(&handle);
}

std::string GetOpCodeString(const char* pAsm, unsigned char* pOpcode, int nSize) {
    std::ostringstream oss;
    for (int i = 0; i < nSize; ++i) {
        oss << std::hex << std::uppercase << (int)pOpcode[i] << " ";
    }
    oss << std::string(30 - nSize * 3, ' ') << pAsm; // Adjust padding based on opcode size
    return oss.str();
}
void AsmEngine::Hex2Asm(unsigned char*CODE , size_t size ,DWORD64 Addr)
{
 int   count = cs_disasm(handle, (unsigned char*)CODE, size , Addr, 0, &insn);
}

std::string AsmEngine::Asm2Hex(std::string AsmCode, DWORD64 Addr) {
    std::string opcodeStr;
    DWORD64 address = Addr;


    // 将 AsmCode 按换行符分割成多条指令
    std::vector<std::string> instructions = splitStringByNewline(AsmCode);

    for (const auto& instr : instructions) {
  /*      if (instr == "endp") {
            break;
        }*/

        strncpy_s(xed.instr, instr.c_str(), sizeof(xed.instr) - 1);
        xed.instr[sizeof(xed.instr) - 1] = '\0';  // 确保字符串以 null 结尾
        xed.cip = address;
        if (XEDPARSE_OK != XEDParseAssemble(&xed)) {
            ASm2HexCode = instr;
            break;
        }
        std::ostringstream oss;
        for (int i = 0; i < xed.dest_size; ++i) {
            oss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (int)xed.dest[i] << " ";
        }

        opcodeStr += oss.str();//GetOpCodeString(xed.instr, xed.dest, xed.dest_size) + "\n";
        address += xed.dest_size;
    }

    return opcodeStr;
}
//
//#define CODE "\x55\x48\x8b\x05\xb8\x13\x00\x00"
//int testPrint()
//{
//    csh handle; // 生成调用capstone API的句柄
//    cs_insn* insn; // 存储指令的详细信息
//    size_t count;
//
//    /**
//     * 初始化cs句柄
//     * arch: 架构类型 (CSARCH)
//     * mode: 硬件模式. CSMODE在cs_mode数据类型中可查
//     * handle: 指向句柄, 返回时更新
//     * return: 创建成功返回CS_ERR_OK，否则返回cs_err枚举中对应的错误信息
//     */
//    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle)) {
//        printf("ERROR: Failed to initialize engine!\n");
//        return -1;
//    }
//
//    /**
//     * 给定缓冲区、大小、地址和编号，反编译机器码
//     * handle: cs_open()返回的句柄
//     * code: 包含要反汇编的机器码的缓冲区。
//     * code_size:上面代码缓冲区的大小。
//     * address:给定原始代码缓冲区中的第一条指令的地址。
//     * insn: 由这个API填写的指令数组。注意: insn将由这个函数分配，应该用cs_free () API释放
//     * count: 需要分解的指令数量，或输入0分解所有指令
//     * return:成功反汇编指令的数量，如果该函数未能反汇编给定的代码，则为0，失败时，调用cs_errno()获取错误代码。
//     */
//    count = cs_disasm(handle, (unsigned char*)CODE, sizeof(CODE) - 1, 0x1000, 0, &insn);
//    if (count) {
//        size_t j;
//
//        for (j = 0; j < count; j++) {
//            printf("0x%""Ix"":\t%s\t\t%s\n", insn[j].address, insn[j].mnemonic, insn[j].op_str);
//        }
//
//        cs_free(insn, count);
//    }
//    else
//        printf("ERROR: Failed to disassemble given code!\n");
//
//    /**
//     * 释放句柄
//     * handle: 指向一个cs_open()打开的句柄
//     * return: 释放成功返回CS_ERR_OK,否则返回cs_err枚举的错误信息
//      */
//    cs_close(&handle);
//	return 0;
//}
//



QStringList byteArrayToStringList(const QByteArray& byteArray) {
    QStringList stringList;
    for (auto byte : byteArray) {
        stringList.append(QString::number(static_cast<unsigned char>(byte), 16).rightJustified(2, '0'));
    }
    return stringList;
}
