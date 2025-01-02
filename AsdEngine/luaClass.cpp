#include"luaClass.h"
#include"LogTools.h"
#include <iostream>
#include <string>
#include <fstream>
#include "Asmtools.h"
#include "winTools.h"
#include "驱动相关/hv.h"
#pragma execution_character_set("utf-8")
using namespace std;

#ifdef _DEBUG // is debug
#pragma comment(lib, "qscintilla2_qt5d.lib")
#else
#pragma comment(lib, "qscintilla2_qt5.lib")
#endif
int LoadHv()
{
    hv::for_each_cpu([&](uint32_t) {
        hv::is_hv_running();
        });

    if (!hv::is_hv_running()) {
        qDebug("HV not running!\n");
        return 0;
    }
    else
    {
        qDebug("load HV success. \n");
        return 1;

    }

}
void test_ept_excute(ULONG64 pid, ULONG64 CodeAddr, ULONG64 WhileAddr) {
    //253C6D70000 - 48 C7 C0 162C0F01     - mov rax,010F2C16 { 17771542 }

    uint32_t status = hv::ept_excute_status::EPTE_DISABLE;
    //hv::ept_excute_finish(&status);
    auto cr3 = hv::query_process_cr3(pid);
    auto tar_phy = hv::get_physical_address(cr3, (void*)WhileAddr);//0x253C6D70014 0x253C6D70042 7FF7571A28C0  0x7FF75718A3D5

    if (!tar_phy)
    {
        qWarning("tar_phy is null\n");

    }

    uint64_t count = 0;


    if (status == hv::ept_excute_status::EPTE_DISABLE)
    {
        hv::for_each_cpu([&](uint32_t) {
            //printf("status:%d\n", status);
            hv::install_ept_remote_excute(cr3, tar_phy, CodeAddr, &status);
            });
    }
    while (status != hv::ept_excute_status::EPTE_DISABLE)
    {
        qDebug("status:%d  count:%lld\n", status, count);
        Sleep(1);
    }


}




int Myadd(int a, int b)
{
    return  a + b;
}
std::string  Asm2Code64(std::string AsmCode, DWORD64 Addr)
{
    AsmEngine ASM(CS_MODE_64);
    string code = ASM.Asm2Hex(AsmCode, Addr);
    if (ASM.xed.error)
    {
        if (ASM.ASm2HexCode.empty())
        {
            spdlog::debug(code.c_str());
        }
        else
        {
            qErrnoWarning("AsmCode: %s    failed: %s\n", ASM.ASm2HexCode.c_str(), ASM.xed.error);
        }
    }
    return code;
}
std::string  Asm2Code32(std::string AsmCode, DWORD64 Addr)
{
    AsmEngine ASM(CS_MODE_32);
    string code = ASM.Asm2Hex(AsmCode, Addr);
    if (ASM.xed.error)
    {
        if (ASM.ASm2HexCode.empty())
        {
            spdlog::debug(code.c_str());
        }
        else
        {
            qErrnoWarning("AsmCode: %s    failed: %s\n", ASM.ASm2HexCode.c_str(), ASM.xed.error);
        }
    }
    return code;
}
void LuaEngine::setLineEditText(const std::string& text)
{
    lineEditText = text;
}

//    WinTools winTools;
//    QList<QPair<int, QString>> processes = winTools.enumerateProcesses();
//    qDebug() << "966666666666666" ;
//    ui.comboBox->clear(); // Clear existing items
//    for (const auto& process : processes) {
//        ui.comboBox->addItem(QString("%1 (%2)").arg(process.second).arg(process.first));
//    }
template<class T>
T MyReadProcessMemory(DWORD32 dwpid, ULONG64 uTargetAddress , DWORD64* isSuccess)
{
    T Buffer = 0;
    if (WinTools::readProcessMemory(dwpid, (VOID*)uTargetAddress, &Buffer, sizeof(T)))
    {
        *isSuccess = 1;
        return Buffer;
    }
    else
    {
        *isSuccess = 0;
        return 0;
    }


}


void LuaEngine::Send(const QString& asmCode)
{
   emit sendToUi(asmCode);
}

void LuaEngine::showTable(const QStringList& headers, const QList<QStringList>& items)
{
    emit sendToTable(headers, items);
}
char* GetString(DWORD32 dwpid, ULONG64 addr ,ULONG64 size)

{
    char data[1000] = { 0 };
    WinTools::readProcessMemory(dwpid, (VOID*)addr, &data, size);
    QString str = QString::fromUtf8(data);

    return data;
}
float readFloat(DWORD32 pid, ULONG64 addr , DWORD64* isSuccess)
{
 
    float flo = MyReadProcessMemory<float>(pid, addr,isSuccess);
    return flo;
}
LuaEngine::LuaEngine() {

    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::io, sol::lib::os);
     lua["package"]["path"] = "lua_scripts\\?.lua;" + lua["package"]["path"].get<std::string>();
     //printTimer = new QTimer(this);
     //connect(printTimer, &QTimer::timeout, this, &LuaEngine::flushPrintBuffer);
     //printTimer->start(50); // 0.05秒
    register_functions();
}

LuaEngine::~LuaEngine() {

}

//void LuaEngine::flushPrintBuffer() {
//    if (!printBuffer.empty()) {
//        std::ostringstream oss;
//        for (const auto& str : printBuffer) {
//            oss << str << std::endl;
//        }
//        spdlog::debug(oss.str().c_str());
//        printBuffer.clear();
//    }
//}

void luaPrintCombinedString(sol::variadic_args va) {
    std::ostringstream oss;
    for (auto v : va) {
        if (v.get_type() == sol::type::nil) {
            oss << "nil";  // 处理 nil 值
        }
        else if (v.get_type() == sol::type::table) {
            sol::table tbl = v;
            oss << "{ ";
            for (auto& kv : tbl) {
                if (kv.second.is<int>()) {
                    oss << kv.second.as<int>() << " ";
                }
                else if (kv.second.is<double>()) {
                    oss << kv.second.as<double>() << " ";
                }
                else if (kv.second.is<std::string>()) {
                    oss << kv.second.as<std::string>() << " ";
                }
                else {
                    // 处理其他类型
                    oss << kv.second.as<std::string>() << " ";
                }
            }
            oss << "}";
        }
        else {
            if (v.is<int>()) {
                oss << v.as<int>();
            }
            else if (v.is<bool>()) {
                oss << v.as<bool>();
            }
            else if (v.is<double>()) {
                oss << v.as<double>();
            }
            else if (v.is<std::string>()) {
                oss << v.as<std::string>();
            }
            else {
                // 处理其他类型
                oss << v.as<std::string>();
            }
        }
        oss << " ";  // 添加分隔符
    }
    static int  num = 0;
    num++;

    spdlog::debug(oss.str().c_str());
}

//send(Asm2Code("push rax" ,0x1000))

bool LuaEngine::GetAGAIN()
{
    return AGAIN;
}

void LuaEngine::register_functions() {
  


    lua.set_function("GetAGAIN", &LuaEngine::GetAGAIN, this);
    lua.set_function("Myadd", &Myadd);
    lua.set_function("print", luaPrintCombinedString);
    lua.set_function("sleep", &Sleep);
    lua.set_function("Asm2Code64", &Asm2Code64);
    lua.set_function("Asm2Code32", &Asm2Code32);
    lua.set_function("LoadHv", &LoadHv);
    lua.set_function("VTCALL", &test_ept_excute);
    lua.set_function("GetString", &GetString);
    lua.set_function("readFloat", &readFloat);

    lua.set_function("showTable", [this](sol::table headers, sol::table items) {
        QStringList qHeaders;
        for (const auto& item : headers) {
            qHeaders << QString::fromStdString(item.second.as<std::string>());
        }

        QList<QStringList> qItems;
        for (const auto& row : items) {
            QStringList qRow;
            for (const auto& item : row.second.as<sol::table>()) {
                qRow << QString::fromStdString(item.second.as<std::string>());
            }
                     qItems << qRow;
        }

        showTable(qHeaders, qItems);
        });



    lua.set_function("Send", [this](const std::string& asmCode) {
        QString qAsmCode = QString::fromStdString(asmCode);
        Send(qAsmCode); });
    // C++函数定义
    lua.set_function("readmem", [this](DWORD32 pid, ULONG64 address, DWORD32 type) -> std::tuple<sol::object, sol::object> {
        DWORD64 isSuccess = 0;
        if (type == 1) {
            BYTE result = MyReadProcessMemory<BYTE>(pid, address, &isSuccess);
            return std::make_tuple(sol::make_object(lua, result), sol::make_object(lua, isSuccess));
        }
        else if (type == 2) {
            DWORD64 result = MyReadProcessMemory<WORD>(pid, address, &isSuccess);
            return std::make_tuple(sol::make_object(lua, result), sol::make_object(lua, isSuccess));
        }
        else if (type == 4) {
            DWORD64 result = MyReadProcessMemory<DWORD32>(pid, address, &isSuccess);
            return std::make_tuple(sol::make_object(lua, result), sol::make_object(lua, isSuccess));
        }
        else if (type == 5) {
            FLOAT result = MyReadProcessMemory<FLOAT>(pid, address, &isSuccess);
            return std::make_tuple(sol::make_object(lua, result), sol::make_object(lua, isSuccess));
        }
        else if (type == 8) {
            DWORD64 result = MyReadProcessMemory<DWORD64>(pid, address, &isSuccess);
            if (result > MAXINT64) {
                return std::make_tuple(sol::make_object(lua, 0), sol::make_object(lua, 2));
            }
            else {
                return std::make_tuple(sol::make_object(lua, result), sol::make_object(lua, isSuccess));
            }
        }
        else {
            return std::make_tuple(sol::nil, sol::nil);
        }
        });

    lua.set_function("writemem", [](DWORD32 pid, ULONG64 address, sol::object value, DWORD32 type) -> bool {
        if (type == 1) {
            BYTE val = value.as<BYTE>();
            return WinTools::writeProcessMemory(pid, (void*)address, &val, sizeof(BYTE));
        }
        else if (type == 2) {
            WORD val = value.as<WORD>();
            return WinTools::writeProcessMemory(pid, (void*)address, &val, sizeof(WORD));
        }
        else if (type == 4) {
            DWORD32 val = value.as<DWORD32>();
            return WinTools::writeProcessMemory(pid, (void*)address, &val, sizeof(DWORD32));
        }
        else if (type == 8) {
            DWORD64 val = value.as<DWORD64>();
            return WinTools::writeProcessMemory(pid, (void*)address, &val, sizeof(DWORD64));
        }
        else {
            return false;
        }
        });

    lua.set_function("allocmem", [](DWORD32 pid, size_t size, DWORD allocationType, DWORD protection) -> ULONG64 {
        return (ULONG64)WinTools::allocateMemory(pid, size, allocationType, protection);
        });

    lua.set_function("changememprot", [](DWORD32 pid, ULONG64 address, size_t size, DWORD newProtection) -> bool {
        DWORD oldProtection;
        return WinTools::changeMemoryProtection(pid, (void*)address, size, newProtection, oldProtection);
        });

    lua.set_function("getpid", [](const std::string& processName) -> int {
        return WinTools::getPidByProcessName(QString::fromStdString(processName));
        });
    lua.set_function("getmodulebase", [](int pid, const std::string& moduleName) -> ULONG64 {
        return (WinTools::getModuleBaseAddress(pid, QString::fromStdString(moduleName)));
        });
    lua.set_function("createremotethread", [](int pid, ULONG64 startAddress, ULONG64 parameter) -> bool {
        return WinTools::createRemoteThread(pid, reinterpret_cast<void*>(startAddress), reinterpret_cast<void*>(parameter));
        });


}



void LuaEngine::RunLuaFile(const char* script) {
    int status = lua.script_file( script);
    if (status == LUA_OK) {
        status = lua_pcall(L, 0, LUA_MULTRET, 0);
    }
    if (status != LUA_OK) {
        spdlog::debug(lua_tostring(L, -1));
        lua_pop(L, 1); // remove error message
    }
}

void LuaEngine::run() {

    if (script.empty())
    {
        try {
            lua.script_file("main1.lua");
        }
        catch (const std::exception& e) {
            emit   errorOccurred(QString::fromStdString(e.what()));
        }

    }
    else
    {
        try {
            lua.script(script);
        }
        catch (const std::exception& e) {
            errorOccurred(QString::fromStdString(e.what()));
        }

    }
   
}

