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
//thread.start(); // �����߳�
// ʹ��ȫ����־��¼����¼��Ϣ

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
    csh handle; // ���ɵ���capstone API�ľ��
    cs_insn* insn; // �洢ָ�����ϸ��Ϣ
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


    // �� AsmCode �����з��ָ�ɶ���ָ��
    std::vector<std::string> instructions = splitStringByNewline(AsmCode);

    for (const auto& instr : instructions) {
  /*      if (instr == "endp") {
            break;
        }*/

        strncpy_s(xed.instr, instr.c_str(), sizeof(xed.instr) - 1);
        xed.instr[sizeof(xed.instr) - 1] = '\0';  // ȷ���ַ����� null ��β
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
//    csh handle; // ���ɵ���capstone API�ľ��
//    cs_insn* insn; // �洢ָ�����ϸ��Ϣ
//    size_t count;
//
//    /**
//     * ��ʼ��cs���
//     * arch: �ܹ����� (CSARCH)
//     * mode: Ӳ��ģʽ. CSMODE��cs_mode���������пɲ�
//     * handle: ָ����, ����ʱ����
//     * return: �����ɹ�����CS_ERR_OK�����򷵻�cs_errö���ж�Ӧ�Ĵ�����Ϣ
//     */
//    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle)) {
//        printf("ERROR: Failed to initialize engine!\n");
//        return -1;
//    }
//
//    /**
//     * ��������������С����ַ�ͱ�ţ������������
//     * handle: cs_open()���صľ��
//     * code: ����Ҫ�����Ļ�����Ļ�������
//     * code_size:������뻺�����Ĵ�С��
//     * address:����ԭʼ���뻺�����еĵ�һ��ָ��ĵ�ַ��
//     * insn: �����API��д��ָ�����顣ע��: insn��������������䣬Ӧ����cs_free () API�ͷ�
//     * count: ��Ҫ�ֽ��ָ��������������0�ֽ�����ָ��
//     * return:�ɹ������ָ�������������ú���δ�ܷ��������Ĵ��룬��Ϊ0��ʧ��ʱ������cs_errno()��ȡ������롣
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
//     * �ͷž��
//     * handle: ָ��һ��cs_open()�򿪵ľ��
//     * return: �ͷųɹ�����CS_ERR_OK,���򷵻�cs_errö�ٵĴ�����Ϣ
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
