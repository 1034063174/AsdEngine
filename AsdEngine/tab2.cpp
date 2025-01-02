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
#include "ReadEtwMsg.h"

Tab2Widget::Tab2Widget(QWidget* parent, Ui::AsdEngineClass ui)
{
    qRegisterMetaType<QMap<QString, QString>>("QMap<QString, QString>");
    UI = ui;
    connect(ui.pushButton_7, &QPushButton::clicked, this, &Tab2Widget::on_button_clicked);



}

void Tab2Widget::showList(QMap<QString, QString> list)
{
    int currentRow = UI.tableWidget->rowCount();
    UI.tableWidget->insertRow(currentRow);
    UI.tableWidget->setItem(currentRow, 0, new QTableWidgetItem( list["ticktack"]));
    UI.tableWidget->setItem(currentRow, 1, new QTableWidgetItem(list["Pid"]));
    UI.tableWidget->setItem(currentRow, 2, new QTableWidgetItem(list["OrgSyscall"]) );
    UI.tableWidget->setItem(currentRow, 3, new QTableWidgetItem(list["FuncName"]));
    
}
void Tab2Widget::on_button_clicked()
{
    ReadEtwMsg* thread = new ReadEtwMsg();

 
        connect(thread, &ReadEtwMsg::SendToTable, this, &Tab2Widget::showList);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);

        thread->start();  // Æô¶¯Ïß³Ì

}
