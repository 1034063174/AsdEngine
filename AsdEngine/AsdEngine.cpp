#include "AsdEngine.h"
#include "qtimer.h"
#include "qdatetime.h"

#include <iostream>
#include <sstream>
#include <string>

#include "Asmtools.h"
#include "log.h"

#include <QThread>
#include <ctime>
#include <filesystem>

#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include "qdir.h"
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
std::shared_ptr<spdlog::logger> qt_logger;

#define QDATETIMS qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))

void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    std::string message = msg.toStdString();
    switch (type) {
    case QtDebugMsg:
        spdlog::debug(message);
        break;
    case QtInfoMsg:
        spdlog::info(message);
        break;
    case QtWarningMsg:
        spdlog::warn(message);
        break;
    case QtCriticalMsg:
        spdlog::error(message);
        break;
    case QtFatalMsg:
        spdlog::critical(message);
        abort();
    }
}

AsdEngine::AsdEngine(QWidget* parent)
    : QMainWindow(parent), updateTimer(new QTimer(this)), tableWindow(new TableWindow(this))
{
    ui.setupUi(this);
    InitLog();

    // �����µ� QsciScintilla 
    textEdit_Lua = new QsciScintilla(this);
    // �к���ʾ
    textEdit_Lua->setMarginType(0, QsciScintilla::NumberMargin);
    textEdit_Lua->setMarginLineNumbers(0, true);
    textEdit_Lua->setMarginWidth(0, 15);

    CustomLexerLua* lexer = new CustomLexerLua(this);
    textEdit_Lua->setLexer(lexer);

    // �� QsciScintilla �ؼ���ӵ�ռλ�� widget ��
    QVBoxLayout* layout = new QVBoxLayout(ui.widgetLua); // ui.widgetLua ��ռλ��
    layout->addWidget(textEdit_Lua);
    ui.widgetLua->setLayout(layout);

    QList<QPushButton*> buttons = {
        findChild<QPushButton*>("Button_changelua1"),
        findChild<QPushButton*>("Button_changelua2"),
        findChild<QPushButton*>("Button_changelua3"),
        findChild<QPushButton*>("Button_changelua4"),
        findChild<QPushButton*>("Button_changelua5"),
        findChild<QPushButton*>("Button_changelua6"),
        findChild<QPushButton*>("fastLuaButton1"),
        findChild<QPushButton*>("fastLuaButton2"),
        findChild<QPushButton*>("fastLuaButton3"),
        findChild<QPushButton*>("fastLuaButton4"),
        findChild<QPushButton*>("fastLuaButton5"),
        findChild<QPushButton*>("fastLuaButton6"),
        findChild<QPushButton*>("fastLuaButton7"),
        findChild<QPushButton*>("fastLuaButton8"),
        findChild<QPushButton*>("fastLuaButton9"),
        findChild<QPushButton*>("fastLuaButton10"),
        findChild<QPushButton*>("fastLuaButton11"),
        findChild<QPushButton*>("fastLuaButton12")
    };

    QStringList buttonNames = {
        "Button_changelua1",
        "Button_changelua2",
        "Button_changelua3",
        "Button_changelua4",
        "Button_changelua5",
        "Button_changelua6",
        "fastLuaButton1",
        "fastLuaButton2",
        "fastLuaButton3",
        "fastLuaButton4",
        "fastLuaButton5",
        "fastLuaButton6",
        "fastLuaButton7",
        "fastLuaButton8",
        "fastLuaButton9",
        "fastLuaButton10",
        "fastLuaButton11",
        "fastLuaButton12"
    };

    // ���������� ComboBox
    QComboBox* comboBox = findChild<QComboBox*>("comboBox");

    buttonManager = new ButtonManager(textEdit_Lua, buttons, buttonNames, comboBox, this);

    // ��ʱ����ʼ��
    connect(updateTimer, &QTimer::timeout, this, &AsdEngine::updateLogToUI);
    connect(updateTimer, &QTimer::timeout, this, &AsdEngine::updateUIWithLogBuffer);
    updateTimer->start(10); // ÿ0.1�봥��һ��

    // ���� comboBox ��ѡ��ı��¼�������ۺ���
    connect(comboBox, &QComboBox::currentTextChanged, this, &AsdEngine::onConfigChanged);

    // ��ʼ�� comboBox
    initializeComboBox();
    //���ӱ��lua�ۺ���

    Tab2Widget* s = new Tab2Widget(this,  ui);


  //  tableWindow->hide();
}

void AsdEngine::initializeComboBox()
{
    initializingComboBox = true;
    QDir configDir("Config");
    if (configDir.exists()) {
        QStringList configFolders = configDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        ui.comboBox->addItems(configFolders);
    }
    initializingComboBox = false;
    QSettings settings("Config/setting.ini", QSettings::IniFormat);
    QString lastConfig = settings.value("LastConfig/Name").toString();
    int index = ui.comboBox->findText(lastConfig);
    if (index != -1) {
        ui.comboBox->setCurrentIndex(index);
    }
}

void AsdEngine::onConfigChanged(const QString& configName)
{
    buttonManager->buttonHandler->loadConfiguration(configName);
    saveCurrentConfig(configName); // ���浱ǰѡ�������
}

void AsdEngine::saveCurrentConfig(const QString& configName)
{
    if (!initializingComboBox) {
        QSettings settings("Config/setting.ini", QSettings::IniFormat);
        settings.setValue("LastConfig/Name", configName);
    }
}

void AsdEngine::startLuaEngine(const QString& script)
{
    qRegisterMetaType<QList<QStringList>>("QList<QStringList>");

    if (!thread) {
        thread = new LuaEngine();
        connect(thread, &LuaEngine::sendToUi, this, &AsdEngine::displayInTextEdit);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);
        connect(thread, &LuaEngine::errorOccurred, this, &AsdEngine::handleException);
        connect(thread, &QThread::finished, this, &AsdEngine::handleThreadFinished);
        connect(thread, &LuaEngine::sendToTable, this, &AsdEngine::uploadTable);  // �����źźͲ�


        thread->script = script.toStdString();
        thread->AGAIN = true;
        thread->start();  // �����߳�
    }
    else 
    {
        qWarning(u8"�߳�û�н������Ƚ����߳�");
    }

}
void AsdEngine::handleThreadFinished()
{
    qDebug(u8"�߳��ѽ���");
    thread = nullptr;
}
void AsdEngine::InitLog()
{
    QDir dir("logput");
    if (!dir.exists()) {
        dir.mkdir("logput");
    }
    // ��־�ļ�·��
    std::string log_filename = std::string("logput/") + QDATETIMS + ".txt";
    // �����̳߳�
    static auto thread_pool = std::make_shared<spdlog::details::thread_pool>(8192, 1);
    // �����ļ���־������
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_filename, 1048576 * 5, 100);
    file_sink->set_level(spdlog::level::debug);
    spdlog::flush_every(std::chrono::seconds(5));

    // ���� Qt ��־������
    auto qt_sink = std::make_shared<QtLogSink>(ui.txtLog);
    connect(qt_sink.get(), &QtLogSink::logMessageAvailable, this, [this](const QString& message) {
        uiLogBuffer.append(message);
        });
    qt_sink->set_level(spdlog::level::debug);

    // ����һ���ַ�������
    auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>();
    dist_sink->add_sink(file_sink);
    dist_sink->add_sink(qt_sink);

    // �����첽��־��¼�������÷ַ�������
    auto async_logger = std::make_shared<spdlog::async_logger>(
        "async_logger", dist_sink, thread_pool, spdlog::async_overflow_policy::block);
    spdlog::register_logger(async_logger);

    spdlog::set_default_logger(async_logger);
    spdlog::set_level(spdlog::level::debug);

    qInstallMessageHandler(customMessageHandler); // �ض���  
}

void AsdEngine::on_Tab1Runlua_clicked()
{
    buttonManager->buttonHandler->saveCurrentContent(); // ����lua���뵽�ļ�
    QString strText = textEdit_Lua->text();
    startLuaEngine(strText);
}


void AsdEngine::on_TableButton_clicked()
{
    if (tableWindow->isVisible()) {
        tableWindow->hide();
    }
    else {
        tableWindow->show();
    //    updateTable();  // ÿ����ʾ����ʱ���±������
    }
}
void AsdEngine::uploadTable(const QStringList& headers, const QList<QStringList>& items)
{
    tableWindow->setTableContent( headers, items);
}

void AsdEngine::on_Tab1CloseLua_clicked()
{
    if (thread) {
        thread->AGAIN = false;
    }
    thread = NULL;
}

void AsdEngine::updateLogToUI()
{
    if (!logBuffer.isEmpty()) {
        ui.textEdit_4->append(logBuffer);
        logBuffer.clear();
    }
}

void AsdEngine::updateUIWithLogBuffer()
{
    if (!uiLogBuffer.isEmpty()) {
        ui.txtLog->append(uiLogBuffer);
        uiLogBuffer.clear();
    }
}

AsdEngine::~AsdEngine()
{
    spdlog::shutdown();
    delete buttonManager;
}
