
#include <QThread>
#include <QTimer>
#include <QMap>
#include <QString>
class ReadEtwMsg : public QThread
{
    Q_OBJECT
public:
    ReadEtwMsg();
    ~ReadEtwMsg();

protected:
    void run() override;
    void Send(QMap<QString, QString> list);
   
private:
    std::vector<std::string> printBuffer;
    std::map<int64_t, std::string> NTintToString;
    std::map<int64_t, std::string> win32intToString;
signals:
    void SendToTable(QMap<QString, QString> list);
};