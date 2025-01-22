#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/async_logger.h>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QApplication>
#include <QTextEdit>
#include "ButtonHandler.h"
#include "winTools.h"
#include <QDebug>
#include "CustomLexerLua.h"
#include <QComboBox>
#include <QSettings>
#include "tab2.h"
#include <QObject>
#include "filterdialog.h"
#include <QDir>
#include <QScrollBar>
#include <QToolTip>
#include "../asd_ETW/asd_ETW/struct.h"
#include"comm/comm.h"
#include <TlHelp32.h>
Tab2Widget::Tab2Widget(QWidget* parent, Ui::AsdEngineClass ui):autoScrollTimer(new QTimer(this))
{
    qRegisterMetaType<QMap<QString, QString>>("QMap<QString, QString>");
    UI = ui;
    connect(UI.pushButton_2, &QPushButton::clicked, this, &Tab2Widget::initDriverAndSymbol);
    connect(UI.pushButton_7, &QPushButton::clicked, this, &Tab2Widget::ETWThreadStart);
    connect(UI.pushButton_5, &QPushButton::clicked, this, &Tab2Widget::on_FilterButton_clicked);
    connect(UI.pushButton_8, &QPushButton::clicked, this, &Tab2Widget::ETWThreadStop);
    connect(UI.comboBox_2, &QComboBox::currentTextChanged, this, &Tab2Widget::ChooseFilter);
    connect(UI.comboBox_2, &MyComboBox::on_ComboBox_clicked, this, &Tab2Widget::initializeComboBox);
    connect(UI.pushButton_4, &QPushButton::clicked, this, &Tab2Widget::CleartableWidget);
    connect(autoScrollTimer, &QTimer::timeout, this, &Tab2Widget::autoScrollToBottom);
    autoScrollTimer->start(50); // 100 ����

    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(UI.radioButton_4, 1);  // 1���� Option 1
    buttonGroup->addButton(UI.radioButton_5, 2);  // 2���� Option 2
    buttonGroup->addButton(UI.radioButton_6, 3);  // 3���� Option 3

    UI.radioButton_4->setToolTip(u8"��ѡ����������κζ�������ʾ���н��̣�����������Ӣ��;����");
    UI.radioButton_5->setToolTip(u8"��ѡ��Ҫ��ֵ�����pid��Ӣ��;����");
    UI.radioButton_6->setToolTip(u8"��ѡ��Ҫ����ע���Сд��notepad.exe,���pid��Ӣ��;����");
    connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onButtonClicked(int)));

    QButtonGroup* buttonGroupf = new QButtonGroup(this);
    buttonGroupf->setExclusive(false);  // ��������ťͬʱѡ��
    buttonGroupf->addButton(UI.radioButton_3);
    buttonGroupf->addButton(UI.radioButton_7);
    buttonGroupf->addButton(UI.radioButton_8);
    buttonGroupf->addButton(UI.radioButton_9);
    buttonGroupf->addButton(UI.radioButton_10);


    connect(UI.pushButton_6, &QPushButton::clicked, this, &Tab2Widget::SetBreakPoint);  //���öϵ�
    connect(UI.pushButton_9, &QPushButton::clicked, this, &Tab2Widget::DeleteBreakPoint);  //ɾ���ϵ�
    connect(UI.pushButton_10, &QPushButton::clicked, this, &Tab2Widget::ContinueBreakPoint);  //�Ź��ϵ�
    connect(UI.pushButton_11, &QPushButton::clicked, this, &Tab2Widget::ReadBreakPointMsg);  //��ȡ��Ϣ



    UI.textEdit->setWordWrapMode(QTextOption::NoWrap);  // �����Զ�����
    UI.textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);  // ���ú��������

    UI.textEdit_2->setWordWrapMode(QTextOption::NoWrap);  // �����Զ�����
    UI.textEdit_2->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);  // ���ú��������

    UI.textEdit_3->setWordWrapMode(QTextOption::NoWrap);  // �����Զ�����
    UI.textEdit_3->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);  // ���ú��������

}
void Tab2Widget::initDriverAndSymbol()
{

    while (g_NTstringToInt.size() <= 1)
    {
        g_NTstringToInt = GetAllFunction("ntoskrnl.exe");
        Sleep(1000);
    }
    while (g_win32stringToInt.size() <= 1)
    {
        g_win32stringToInt = GetAllFunction("win32k.sys");
        Sleep(1000);
    }
}

void Tab2Widget::SetBreakPoint()
{
    QString text = UI.textEdit->toPlainText();
    QStringList lines = text.split('\n');
    QMap<QString, QString> dictionary;
    for (int i = 0; i < lines.size(); ++i) {

        QStringList Temp = lines[i].split("=");
        dictionary.insert(Temp[0], Temp[1]);  // ���к���Ϊ�����ı�����Ϊֵ
    }
    Sleep(1);
    BreakPointData data = {0};
    memset(&data, 0, sizeof(data));
    Sleep(1);

    if (dictionary.contains("API"))
    {
        data.Address = g_NTstringToInt[ (dictionary["API"]).toStdString()];

        if (!data.Address)
        {
            data.Address = g_win32stringToInt[(dictionary["API"]).toStdString()];
        }
    }
    if (dictionary.contains("R8"))
    {
        data.R8 = dictionary["R8"].toInt();
        data.UseR8 = TRUE;
    }
    if (dictionary.contains("R9"))
    {
        data.R9 = dictionary["R9"].toInt();
        data.UseR9 = TRUE;
    }
    if (dictionary.contains("RCX"))
    {
        data.RCX = dictionary["RCX"].toInt();
        data.UseRCX = TRUE;
    }
    if (dictionary.contains("RDX"))
    {
        data.RDX = dictionary["RDX"].toInt();
        data.UseRDX = TRUE;
    }
    if (dictionary.contains("PID"))
    {
        data.Pid = dictionary["PID"].toInt();
        g_pid = (HANDLE)dictionary["PID"].toInt();
    }
    DriverComm(ASD_SET_BREAK_POINT, &data, sizeof(data));
}

void Tab2Widget::DeleteBreakPoint()
{
    DriverComm(ASD_DELETE_BREAK_POINT, 0, 0);
}
using std::vector;
struct  CModuleMsg
{
    std::string sModulename;
    DWORD64 qwModuleSize;
    DWORD64 qwModuleAddress;
};
typedef struct __MyModuleMsg
{

    ULONGLONG nllImageBaseAddress;
    ULONGLONG nllSize;
    char  szModuleName[0x50];

}MyModuleMsg, * PMyModuleMsg;
int GetAllModules(HANDLE dwProcessId, vector<CModuleMsg>& aProcessMoudles)
{
    aProcessMoudles.clear();//�������

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, (DWORD)dwProcessId); // �������̿���
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf(" %d ,  Failed to create snapshot. \r\n ", dwProcessId);
        return -1;
    }

    MODULEENTRY32 CmoduleEntry;
    CmoduleEntry.dwSize = sizeof(CmoduleEntry);

    BOOL bResult = Module32First(hSnapshot, &CmoduleEntry); // ������һ��ģ��
    while (bResult) {
        std::wstring wsModuleNameWide(CmoduleEntry.szModule); // ��ANSI�ַ���ת��Ϊ���ַ��ַ���

        int iBufferLength = WideCharToMultiByte(CP_UTF8, 0, wsModuleNameWide.data(), -1, nullptr, 0, nullptr, nullptr);
        char* szModuleNameBuffer = (char*)malloc(iBufferLength);
        WideCharToMultiByte(CP_UTF8, 0, wsModuleNameWide.data(), -1, szModuleNameBuffer, iBufferLength, nullptr, nullptr);

        CModuleMsg Cmsg;


        Cmsg.qwModuleAddress = (DWORD64)CmoduleEntry.modBaseAddr;
        Cmsg.qwModuleSize = CmoduleEntry.modBaseSize;
        Cmsg.sModulename = szModuleNameBuffer;
        aProcessMoudles.push_back(Cmsg);

        free(szModuleNameBuffer);

        bResult = Module32Next(hSnapshot, &CmoduleEntry); // ������һ��ģ��
    }

    CloseHandle(hSnapshot); // �رտ��վ��

    return 0;

}
void Tab2Widget::ContinueBreakPoint()
{
    ULONG64 stack[0x100];
    RtlZeroMemory(stack, 0x100 * sizeof(ULONG64));
    DriverComm(ASD_RUN_BREAK_POINT, &stack[0], 0x100);
    UI.textEdit_3->clear();  // ��� QTextEdit ����
    vector<CModuleMsg> aProcessMoudles;
    int currentRow = UI.tableWidget->rowCount();
    GetAllModules(g_pid, aProcessMoudles);

    for (ULONG64 value : stack)
    {
        if (value == 0)
        {
            break;  // ������� 0����ֹͣ
        }
        do {
            BOOLEAN isshow=FALSE;
            for (int row = 0; row < aProcessMoudles.size(); row++)
            {
                ULONG64 sz = aProcessMoudles[row].qwModuleSize;
                ULONG64  maxaddress = (ULONG64)sz + aProcessMoudles[row].qwModuleAddress;

                if (aProcessMoudles[row].qwModuleAddress < value && maxaddress>value)
                {
                    QString str = QString::number(value, 16);  // ת��Ϊʮ��������ʾ
                    QString qStr = QString::fromStdString(aProcessMoudles[row].sModulename);

                    UI.textEdit_3->append(str + "  " + qStr + " + " + QString::number(value - aProcessMoudles[row].qwModuleAddress, 16));  // ÿ��ֵռһ��
                    isshow = TRUE;
                    break;

                }
            }
            if (!isshow)
            {
                QString str = QString::number(value, 16);  // ת��Ϊʮ��������ʾ
                UI.textEdit_3->append(str);  // ÿ��ֵռһ��
            }

        } while (0);

        // �� ULONG64 ��ֵתΪ QString

    }
}

void Tab2Widget::ReadBreakPointMsg()
{
    BreakPointMessage  data = {0};
    DriverComm(ASD_GET_BREAK_POINT_MESSAGE  , &data, sizeof(BreakPointMessage));

    UI.textEdit_2->clear();  // ��� QTextEdit ����


        // �� ULONG64 ��ֵתΪ QString
    QString str = QString::number(data.RCX, 16);  // ת��Ϊʮ��������ʾ
     UI.textEdit_2->append("RCX = "+str);  // ÿ��ֵռһ��
      str = QString::number(data.RDX, 16);  // ת��Ϊʮ��������ʾ
     UI.textEdit_2->append("RDX = " + str);  // ÿ��ֵռһ��
      str = QString::number(data.R8, 16);  // ת��Ϊʮ��������ʾ
     UI.textEdit_2->append(" R8 = " + str);  // ÿ��ֵռһ��
      str = QString::number(data.R9, 16);  // ת��Ϊʮ��������ʾ
     UI.textEdit_2->append(" R9 = " + str);  // ÿ��ֵռһ��
}

void Tab2Widget::autoScrollToBottom()
{
    if (UI.radioButton_3->isChecked())
    {

        int lastRow = UI.tableWidget->rowCount() - 1;

        // ��ȡ���������
        QScrollBar* verticalScrollBar = UI.tableWidget->verticalScrollBar();

        // ���������һ�У������������
        verticalScrollBar->setValue(verticalScrollBar->maximum());
    }
 

}

void  Tab2Widget::CleartableWidget()
{

    UI.tableWidget->clearContents();
    UI.tableWidget->setRowCount(0);

}


void Tab2Widget::initializeComboBox()
{
    QDir configDir(QString("Config/ETW/filter"));
    if (configDir.exists()) {
        QStringList configFolders = configDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        UI.comboBox_2->clear();
        UI.comboBox_2->addItems(configFolders);
    }
}

void  Tab2Widget::ChooseFilter(const  QString& name)
{
    static bool isfirst = true;
    if (isfirst)
    {
        isfirst = ~isfirst;


        QString filename = QString("%1/%2").arg(QString("Config/ETW/filter")).arg(name);
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in.setCodec("UTF-8");  // ���ñ����ʽΪ UTF-8
            strFilter = in.readAll();
           // qDebug() << ddd.toUtf8();
            file.close();
        }
        //else {
        //    qDebug(u8"����");

        //}

    }

}

void Tab2Widget::showList(QMap<QString, QString> list)
{
    int currentRow = UI.tableWidget->rowCount();
    UI.tableWidget->insertRow(currentRow);
    UI.tableWidget->setItem(currentRow, 0, new QTableWidgetItem( list["ticktack"]));
    UI.tableWidget->setItem(currentRow, 1, new QTableWidgetItem(list["ProcessName"]));
    UI.tableWidget->setItem(currentRow, 2, new QTableWidgetItem(list["Pid"]));
    UI.tableWidget->setItem(currentRow, 3, new QTableWidgetItem(list["FuncName"]));
    UI.tableWidget->setItem(currentRow, 5, new QTableWidgetItem(list["rcx"]));
    UI.tableWidget->setItem(currentRow, 6, new QTableWidgetItem(list["rdx"]));
    UI.tableWidget->setItem(currentRow, 7, new QTableWidgetItem(list["r8"]));
    UI.tableWidget->setItem(currentRow, 8, new QTableWidgetItem(list["r9"]));
}
void Tab2Widget::ETWThreadStart()
{

    QVector<int> m_includePid = {};        // �����Ľ��� PID
    QVector<int> m_excludePid = {}; // �ų��Ľ��� PID
    QVector<QString> m_processName = {};
    if (SelectedOptionID == 1)
    {
        for (const QString& line : UI.lineEdit->text().remove(' ').split(';', QString::SkipEmptyParts).toVector()) {
            bool ok;
            if (int value = line.toInt(&ok); ok) {
                m_includePid.append(value);  // ���ת���ɹ�������� QVector<int>
            }
            else {
                qWarning() << "Conversion failed for" << line;
            }
        }
    }
    else if (SelectedOptionID == 2)
    {

        for (const QString& line : UI.lineEdit_2->text().remove(' ').split(';', QString::SkipEmptyParts).toVector()) {
            bool ok;
            if (int value = line.toInt(&ok); ok) {
                m_excludePid.append(value);  // ���ת���ɹ�������� QVector<int>
            }
            else {
                qWarning() << "Conversion failed for" << line;
            }
        }
    }
    else if (SelectedOptionID == 3)
    {
        m_processName = UI.lineEdit_3->text().remove(' ').split(';', QString::SkipEmptyParts).toVector();;   // �����Ľ�������
    }

    QVector<QString> m_includeAPI = strFilter.remove(' ').split('\n', QString::SkipEmptyParts).toVector();

    threadConfig config;
    config.show_ssdt = UI.radioButton_7->isChecked();
    config.show_sssdt = UI.radioButton_8->isChecked();
    config.show_stack = UI.radioButton_9->isChecked();
    config.show_Registers_Data = UI.radioButton_10->isChecked();

    thread = new ReadEtwMsg(m_includePid, m_excludePid, m_processName, m_includeAPI, SelectedOptionID , config, g_NTstringToInt, g_win32stringToInt);
    connect(thread, &ReadEtwMsg::SendToTable, this, &Tab2Widget::showList);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();  // �����߳�

}

void Tab2Widget::ETWThreadStop()
{
    if (thread) {
        thread->AGAIN = false;
    }
    thread = NULL;
}


void Tab2Widget::on_FilterButton_clicked()
{
    // �����´���ʵ��
    FilterDialog dialog(this);

    // ��ʾ����
    dialog.exec(); // ʹ�� exec() ��ģ̬�Ի��򣬻�������ǰ����

}

