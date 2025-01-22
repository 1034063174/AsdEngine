#include "filterdialog.h"
#include <QInputDialog>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include<QDir>
#include"mycomboBox.h"
FilterDialog::FilterDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::Dialog) {  // ʹ�� Ui::Dialog
    ui->setupUi(this);  // ��ʼ�� UI
    currentConfigPath = QString("Config/ETW/filter");
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &FilterDialog::ChooseFilterFile);
    connect(ui->comboBox, &MyComboBox::on_ComboBox_clicked, this, &FilterDialog::initializeComboBox);
    connect(ui->pushButtonOK, &QPushButton::clicked, this, &FilterDialog::saveContentToFile);

   // ui->comboBox->addItem("123");
}

FilterDialog::~FilterDialog() {
    delete ui;  // �ͷ� UI
}
void FilterDialog::on_pushButtonCreate_clicked() {
    bool ok;
    QString text = QInputDialog::getText(nullptr, u8"�½�����", u8"�����������:", QLineEdit::Normal, "", &ok);

    // ������ȷ������ʾ���������
    if (ok && !text.isEmpty()) {
        saveRulFile(text);
       qDebug( "%s\r\n",text);
    }
}

void FilterDialog::saveRulFile(QString name )
{
    if (currentConfigPath.isEmpty()) {
        qDebug() << u8"δ��������·��.";
        return;
    }
    QString filename = QString("%1/%2.txt").arg(currentConfigPath).arg(name);

    QFile file(filename);

    if (file.exists()) {
        qDebug() << u8"�ļ�����";
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out.setCodec("UTF-8");
            out << ui->textEdit->toPlainText();
            file.close();
        }
    }
    else {
        qDebug() << u8"�ļ�������";
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
        qDebug() << u8"δ��������·��.";
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
            qDebug() << u8"δ��������·��.";
            return;
        }

        QString filename = QString("%1/%2").arg(currentConfigPath).arg(name);
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in.setCodec("UTF-8");  // ���ñ����ʽΪ UTF-8
            ui->textEdit->setPlainText(in.readAll());
            file.close();
        }
        else {
            ui->textEdit->setPlainText(""); // ����ļ������ڣ����ʼ��Ϊ���ַ���
        }

    }
    
}
