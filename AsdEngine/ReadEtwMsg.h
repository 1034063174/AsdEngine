
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
    bool AGAIN;  //����AGNAIN�ǽ������� ͨ������  while��AGNAIN��


protected:
    void run() override;
    void Send(QMap<QString, QString> list);

private:
    int chooseModule = 0;
    std::vector<std::string> printBuffer;
    std::map<int64_t, std::string> NTintToString;
    std::map<int64_t, std::string> win32intToString;
    QVector<int> m_includePid = {};        // �����Ľ��� PID
    QVector<int> m_excludePid = {};        // �ų��Ľ��� PID
    QVector<QString> m_processName = {};   // �����Ľ�������
    QVector<QString> m_includeAPI = {};    // ������ API

    bool m_show_ssdt = false; //��ʾssdt
    bool m_show_sssdt = false;// sssdt
    bool m_show_stack = false;//��ʾ���ö�ջ
    bool m_show_Registers_Data = false;//��ӡ�Ĵ�������ֵ




signals:
    void SendToTable(QMap<QString, QString> list);
};
