#pragma once
extern "C"
{
#include "lua.h"
#include"lualib.h"
#include"lauxlib.h"
}
#define SOL_ALL_SAFETIES_ON 1
#include <sol\sol.hpp> // or #include "sol.hpp", whichever suits your needs
#include <QThread>
#include <QTimer>
class LuaEngine : public QThread
{
    Q_OBJECT
public:
    LuaEngine();
    ~LuaEngine();
    void flushPrintBuffer();
    bool AGAIN ;  //这里AGNAIN是接着运行 通常用于  while（AGNAIN）
public:
    void RunLuaFile(const char* script);
    void register_functions();
    bool GetAGAIN();
    std::string  scriptFile;
    std::string  script;
    void setLineEditText(const std::string& text);  // 新增：设置lineEdit文本内容

protected:
    void run() override;
private:
    std::vector<std::string> printBuffer;
    sol::state lua;
    int EngineNum;
    lua_State* L; 
    std::string lineEditText;  // 新增：存储lineEdit文本内容
    void  Send(const QString& asmCode);

    void showTable(const QStringList& headers, const QList<QStringList>& items);

 //   void GetStringssdasd(DWORD32 dwpid, ULONG64 addr, ULONG64 size);

    QTimer* printTimer;

signals:
    void sendToUi(const QString& asmCode);
    void sendToTable(const QStringList& headers, const QList<QStringList>& items   );



signals:
    void errorOccurred(const QString& message);
};

