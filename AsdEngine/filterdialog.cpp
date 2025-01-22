#include "filterdialog.h"
#include <QInputDialog>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include<QDir>
#include"mycomboBox.h"
FilterDialog::FilterDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::Dialog) {  // 使用 Ui::Dialog
    ui->setupUi(this);  // 初始化 UI
    currentConfigPath = QString("Config/ETW/filter");
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &FilterDialog::ChooseFilterFile);
    connect(ui->comboBox, &MyComboBox::on_ComboBox_clicked, this, &FilterDialog::initializeComboBox);
    connect(ui->pushButtonOK, &QPushButton::clicked, this, &FilterDialog::saveContentToFile);

   // ui->comboBox->addItem("123");
}

FilterDialog::~FilterDialog() {
    delete ui;  // 释放 UI
}
void FilterDialog::on_pushButtonCreate_clicked() {
    bool ok;
    QString text = QInputDialog::getText(nullptr, u8"新建规则", u8"输入规则名称:", QLineEdit::Normal, "", &ok);

    // 如果点击确定，显示输入的内容
    if (ok && !text.isEmpty()) {
        saveRulFile(text);
       qDebug( "%s\r\n",text);
    }
}

void FilterDialog::saveRulFile(QString name )
{
    if (currentConfigPath.isEmpty()) {
        qDebug() << u8"未设置配置路径.";
        return;
    }
    QString filename = QString("%1/%2.txt").arg(currentConfigPath).arg(name);

    QFile file(filename);

    if (file.exists()) {
        qDebug() << u8"文件存在";
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out.setCodec("UTF-8");
            out << ui->textEdit->toPlainText();
            file.close();
        }
    }
    else {
        qDebug() << u8"文件不存在";
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out.setCodec("UTF-8");
            out << "";
            file.close();
        }
    } 
}



void FilterDialog::initializeComboBox()
{
    QDir configDir(currentConfigPath);
    if (configDir.exists()) {
        QStringList configFolders = configDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
        ui->comboBox->clear();
        ui->comboBox->addItems(configFolders);
    }
}


void FilterDialog::saveContentToFile()
{
    if (currentConfigPath.isEmpty()) {
        qDebug() << u8"未设置配置路径.";
        return;
    }
    QString filename = QString("%1/%2").arg(currentConfigPath).arg(ui->comboBox->currentText());
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << ui->textEdit->toPlainText();
        file.close();
    }
}

void  FilterDialog::ChooseFilterFile(const  QString &name)
{
    static bool isfirst = true;
    if (isfirst)
    {
        isfirst = ~isfirst;


        if (currentConfigPath.isEmpty()) {
            qDebug() << u8"未设置配置路径.";
            return;
        }

        QString filename = QString("%1/%2").arg(currentConfigPath).arg(name);
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in.setCodec("UTF-8");  // 设置编码格式为 UTF-8
            ui->textEdit->setPlainText(in.readAll());
            file.close();
        }
        else {
            ui->textEdit->setPlainText(""); // 如果文件不存在，则初始化为空字符串
        }

    }
    
}
