
#include <QThread>
#include <QTimer>
#include <QMap>
#include <QString>
#include <QVector>

typedef struct _threadConfig
{
    bool show_ssdt;
    bool show_sssdt;
    bool show_stack;
    bool show_Registers_Data;
}threadConfig, * PthreadConfig;


class ReadEtwMsg : public QThread
{
    Q_OBJECT
public:
    ReadEtwMsg(QVector<int>& includePid, QVector<int>& ExcludePid, QVector<QString>& ProcessName, QVector<QString>& includeAPI ,int chooseModule , threadConfig config , std::map<std::string, int64_t>  g_NTstringToInt ,std::map<std::string, int64_t>  g_win32stringToInt);

    ~ReadEtwMsg();
    bool AGAIN;  //这里AGNAIN是接着运行 通常用于  while（AGNAIN）


protected:
    void run() override;
    void Send(QMap<QString, QString> list);

private:
    int chooseModule = 0;
    std::vector<std::string> printBuffer;
    std::map<int64_t, std::string> NTintToString;
    std::map<int64_t, std::string> win32intToString;
    QVector<int> m_includePid = {};        // 包含的进程 PID
    QVector<int> m_excludePid = {};        // 排除的进程 PID
    QVector<QString> m_processName = {};   // 包含的进程名称
    QVector<QString> m_includeAPI = {};    // 包含的 API

    bool m_show_ssdt = false; //显示ssdt
    bool m_show_sssdt = false;// sssdt
    bool m_show_stack = false;//显示调用堆栈
    bool m_show_Registers_Data = false;//打印寄存器的数值




signals:
    void SendToTable(QMap<QString, QString> list);
};
