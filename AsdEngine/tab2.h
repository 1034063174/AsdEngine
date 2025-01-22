#ifndef TAB2WIDGET_H
#define TAB2WIDGET_H

#include <QWidget>
#include <QPushButton>
#include "AsdEngine.h"  // ÒýÈë MainWindow
#include "ReadEtwMsg.h"
#include <QTimer>
#include "tools/GetPdb.h"
class Tab2Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Tab2Widget(QWidget* parent, Ui::AsdEngineClass ui);

private slots:
    void ETWThreadStart();
    void ETWThreadStop();
    void on_FilterButton_clicked();
    void ChooseFilter(const  QString& name);
    void initializeComboBox();
    void CleartableWidget();
    void onButtonClicked(int id) {
        SelectedOptionID = id;
    }
    void SetBreakPoint();
    void DeleteBreakPoint();
    void ContinueBreakPoint();
    void ReadBreakPointMsg();
    void initDriverAndSymbol();

private:
    void showList(QMap<QString, QString> list);
    QPushButton* m_button;
    Ui::AsdEngineClass UI;
    std::map<int64_t, std::string> intToString;
    ReadEtwMsg* thread ;
    QString strFilter;
    void autoScrollToBottom();
    QTimer* autoScrollTimer;
    int SelectedOptionID = 0;
    std::map<std::string, int64_t>  g_NTstringToInt;
    std::map<std::string, int64_t>  g_win32stringToInt;
    HANDLE g_pid = 0;
};

#endif // TAB2WIDGET_H
