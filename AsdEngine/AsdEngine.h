#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AsdEngine.h"
#include <QMessageBox>
#include <spdlog/spdlog.h>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>
#include "ButtonManager.h"
#include <luaClass.h>
#include <QTimer>
#include "tablewindow.h"
class AsdEngine : public QMainWindow
{
    Q_OBJECT

public:
    AsdEngine(QWidget* parent = nullptr);
    ~AsdEngine();

    void InitLog();
    void startLuaEngine(const QString& script);
    void handleThreadFinished();
    QTextEdit* getTextEdit() const;

private slots:
    void on_Tab1Runlua_clicked();
    void on_Tab1CloseLua_clicked();
    void on_TableButton_clicked();
   // void  on_pushButton_clicked();
    void updateLogToUI();
    void updateUIWithLogBuffer();
    void onConfigChanged(const QString& configName);

    void saveCurrentConfig(const QString& configName);

    // Error handling
    void handleException(const QString& message) {
        spdlog::error(message.toStdString());
    }

    // Log display
    void displayInTextEdit(const QString& asmCode) {
        logBuffer.append(asmCode + "\n");
    }
    void uploadTable(const QStringList& headers, const QList<QStringList>& items);

private:
    void create_log_directory();
    void manage_log_files();
    void initializeComboBox();

    Ui::AsdEngineClass ui;
    QsciScintilla* textEdit_Lua;  // 指向 QsciScintilla 的指针
    QsciLexerLua* lexer;
    ButtonManager* buttonManager; // 指向 ButtonManager 的指针

    QTimer* updateTimer;
    QString logBuffer;
    QString uiLogBuffer;
    LuaEngine* thread;
     TableWindow* tableWindow;
    bool initializingComboBox;
   
};
