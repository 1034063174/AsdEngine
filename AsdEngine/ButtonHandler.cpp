 #include "ButtonHandler.h"
#include <QDir>
#include <QDebug>
#include <QInputDialog>
#include <QEvent>
#include <QMouseEvent>
#include <QMenu>
#include "AsdEngine.h"
#include <QSettings>
#include <QProcess>
ButtonHandler::ButtonHandler(QsciScintilla* scintilla, QList<QPushButton*> buttons, QStringList buttonNames, QComboBox* comboBox, AsdEngine* engine, QObject* parent)
    : QObject(parent), scintilla(scintilla), currentButtonIndex(-1), buttons(buttons), buttonNames(buttonNames), comboBox(comboBox), engine(engine)
{
    QDir dir("Config");
    if (!dir.exists()) {
        if (!dir.mkdir(".")) {
            qDebug() << u8"����Ŀ¼ʧ��: " << dir.absolutePath();
        }
        else {
            qDebug() << u8"Ŀ¼�Ѵ���: " << dir.absolutePath();
        }
    }
    else {
        qDebug() << u8"Ŀ¼�Ѵ���: " << dir.absolutePath();
    }

    // ��ʼ�� contentMap �����ļ���ȡ����
    loadConfiguration(comboBox->currentText());

    // Ϊÿ����ť��װ�¼������������������Ĳ˵�
    for (auto button : buttons) {
        button->installEventFilter(this);
        button->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(button, &QPushButton::customContextMenuRequested, this, &ButtonHandler::showContextMenu);
    }

    // ���� comboBox ���źŵ� loadConfiguration �ۺ���
    connect(comboBox, &QComboBox::currentTextChanged, this, &ButtonHandler::loadConfiguration);
}

void ButtonHandler::saveCurrentContent()
{
    if (currentButtonIndex != -1) {
        contentMap[currentButtonIndex] = scintilla->text();
        saveContentToFile(currentButtonIndex); // �������ݵ��ļ�
    }
}

void ButtonHandler::loadContent(int buttonIndex)
{
    scintilla->setText(contentMap[buttonIndex]);
    currentButtonIndex = buttonIndex;
    updateButtonStyles(buttonIndex);
}

void ButtonHandler::updateButtonStyles(int activeButtonIndex)
{
    for (int i = 0; i < buttons.size(); ++i) {
        if (i == activeButtonIndex - 1) {
            buttons[i]->setStyleSheet("background-color: gray;");
        }
        else {
            buttons[i]->setStyleSheet("");
        }
    }
}

void ButtonHandler::saveContentToFile(int buttonIndex)
{
    if (currentConfigPath.isEmpty()) {
        qDebug() << "δ��������·��.";
        return;
    }
    QString filename = QString("%1/content_%2.lua").arg(currentConfigPath).arg(buttonIndex);
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << contentMap[buttonIndex];
        file.close();
    }
}

void ButtonHandler::loadContentFromFile()
{
    if (currentConfigPath.isEmpty()) {
        qDebug() << "δ��������·��.";
        return;
    }
    for (int i = 1; i <= buttons.size(); ++i) {
        QString filename = QString("%1/content_%2.lua").arg(currentConfigPath).arg(i);
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in.setCodec("UTF-8");  // ���ñ����ʽΪ UTF-8
            contentMap[i] = in.readAll();
            file.close();
        }
        else {
            contentMap[i] = ""; // ����ļ������ڣ����ʼ��Ϊ���ַ���
        }
    }
}

void ButtonHandler::saveButtonNames()
{
    if (currentConfigPath.isEmpty()) {
        qDebug() << "δ��������·��.";
        return;
    }
    QSettings settings(currentConfigPath + "/settings.ini", QSettings::IniFormat);
    for (int i = 0; i < buttons.size(); ++i) {
        settings.setValue(QString("ButtonNames/%1").arg(buttonNames[i]), buttons[i]->text());
    }
}

void ButtonHandler::loadButtonNames()
{
    if (currentConfigPath.isEmpty()) {
        qDebug() << "δ��������·��.";
        return;
    }
    QSettings settings(currentConfigPath + "/settings.ini", QSettings::IniFormat);
    for (int i = 0; i < buttons.size(); ++i) {
        QString name = settings.value(QString("ButtonNames/%1").arg(buttonNames[i]), buttonNames[i]).toString();
        buttons[i]->setText(name);
    }
}

void ButtonHandler::buttonclicked(int buttonIndex)
{
    saveCurrentContent();
    loadContent(buttonIndex);
    currentButton = buttons[buttonIndex - 1];  // ���浱ǰ��ť
}

bool ButtonHandler::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        for (int i = 0; i < buttons.size(); ++i) {
            if (obj == buttons[i]) {
                bool ok;
                QString newText = QInputDialog::getText(buttons[i], tr(u8"�༭��ť����"),
                    tr(u8"�����µİ�ť����:"), QLineEdit::Normal,
                    buttons[i]->text(), &ok);
                if (ok && !newText.isEmpty()) {
                    buttons[i]->setText(newText);
                    saveButtonNames(); // ���水ť���Ƶ��ļ�
                }
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void ButtonHandler::showContextMenu(const QPoint& pos)
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        currentButton = button;  // ��¼��ǰ�Ҽ������İ�ť
        QMenu contextMenu(button);
        QAction* print1Action = contextMenu.addAction(u8"��ť��");
        QAction* print2Action = contextMenu.addAction(u8"ִ��");
        QAction* print3Action = contextMenu.addAction(u8"VsCode��");
        connect(print1Action, &QAction::triggered, this, &ButtonHandler::handlePrint1);
        connect(print2Action, &QAction::triggered, this, &ButtonHandler::handlePrint2);
        connect(print3Action, &QAction::triggered, this, &ButtonHandler::handlePrint3);
        contextMenu.exec(button->mapToGlobal(pos));
    }
}

void ButtonHandler::handlePrint1()
{
    if (currentButton) {
        qDebug() << currentButton->text();
    }
    else {
        qDebug() << "δѡ��ť.";
    }
}

void ButtonHandler::handlePrint2()
{
    if (currentButton) {
        int buttonIndex = buttons.indexOf(currentButton) + 1;
        if (contentMap.contains(buttonIndex)) {
            QString content = contentMap[buttonIndex];

            engine->startLuaEngine(content);
            qDebug() << content;
        }
        else {
            qDebug() << "��ť����û������: " << buttonIndex;
        }
    }
    else {
        qDebug() << "δѡ��ť.";
    }
}


void ButtonHandler::handlePrint3()
{
    if (currentButton) {
        qDebug() << currentButton->text();
    }
    else {
        qDebug() << "δѡ��ť.";
    }
    if (currentButton) {
        int buttonIndex = buttons.indexOf(currentButton) + 1;
        QString filename = QString("%1/content_%2.lua").arg(currentConfigPath).arg(buttonIndex);


        qDebug() << u8"��·��: " << filename;
        QString program = "F:\\Microsoft VS Code\\Code.exe";

        QStringList arguments;
        arguments << filename;  // Replace with the path to your code

        // Start the process
        QProcess* process = new QProcess(this);
        connect(process, &QProcess::errorOccurred, [](QProcess::ProcessError error) {
            qDebug() << "Error occurred:" << error;
            });
        process->start(program, arguments);

    }
    else {
        qDebug() << "δѡ��ť.";
    }
}


void ButtonHandler::loadConfiguration(const QString& configName)
{
    currentConfigPath = QString("Config/%1").arg(configName);
    QDir dir(currentConfigPath);
    if (!dir.exists()) {
        qDebug() << "����Ŀ¼������: " << currentConfigPath;
        if (!dir.mkpath(currentConfigPath)) {
            qDebug() << "��������Ŀ¼ʧ��: " << currentConfigPath;
            return;
        }
    }

    // ��ʼ�� contentMap
    contentMap.clear();
    for (int i = 1; i <= buttons.size(); ++i) {
        contentMap[i] = "";
    }

    // ���ذ�ť����
    loadButtonNames();

    // ����ÿ����ť������
    loadContentFromFile();

    // ����а�ť��Ĭ�ϼ��ص�һ����ť�����ݲ�������ʽ
    if (!buttons.isEmpty()) {
        loadContent(1);
        updateButtonStyles(1);
    }
}
