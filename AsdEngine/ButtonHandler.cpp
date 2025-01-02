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
            qDebug() << u8"创建目录失败: " << dir.absolutePath();
        }
        else {
            qDebug() << u8"目录已创建: " << dir.absolutePath();
        }
    }
    else {
        qDebug() << u8"目录已存在: " << dir.absolutePath();
    }

    // 初始化 contentMap 并从文件读取内容
    loadConfiguration(comboBox->currentText());

    // 为每个按钮安装事件过滤器并设置上下文菜单
    for (auto button : buttons) {
        button->installEventFilter(this);
        button->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(button, &QPushButton::customContextMenuRequested, this, &ButtonHandler::showContextMenu);
    }

    // 连接 comboBox 的信号到 loadConfiguration 槽函数
    connect(comboBox, &QComboBox::currentTextChanged, this, &ButtonHandler::loadConfiguration);
}

void ButtonHandler::saveCurrentContent()
{
    if (currentButtonIndex != -1) {
        contentMap[currentButtonIndex] = scintilla->text();
        saveContentToFile(currentButtonIndex); // 保存内容到文件
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
        qDebug() << "未设置配置路径.";
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
        qDebug() << "未设置配置路径.";
        return;
    }
    for (int i = 1; i <= buttons.size(); ++i) {
        QString filename = QString("%1/content_%2.lua").arg(currentConfigPath).arg(i);
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in.setCodec("UTF-8");  // 设置编码格式为 UTF-8
            contentMap[i] = in.readAll();
            file.close();
        }
        else {
            contentMap[i] = ""; // 如果文件不存在，则初始化为空字符串
        }
    }
}

void ButtonHandler::saveButtonNames()
{
    if (currentConfigPath.isEmpty()) {
        qDebug() << "未设置配置路径.";
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
        qDebug() << "未设置配置路径.";
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
    currentButton = buttons[buttonIndex - 1];  // 保存当前按钮
}

bool ButtonHandler::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        for (int i = 0; i < buttons.size(); ++i) {
            if (obj == buttons[i]) {
                bool ok;
                QString newText = QInputDialog::getText(buttons[i], tr(u8"编辑按钮名称"),
                    tr(u8"输入新的按钮名称:"), QLineEdit::Normal,
                    buttons[i]->text(), &ok);
                if (ok && !newText.isEmpty()) {
                    buttons[i]->setText(newText);
                    saveButtonNames(); // 保存按钮名称到文件
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
        currentButton = button;  // 记录当前右键单击的按钮
        QMenu contextMenu(button);
        QAction* print1Action = contextMenu.addAction(u8"按钮名");
        QAction* print2Action = contextMenu.addAction(u8"执行");
        QAction* print3Action = contextMenu.addAction(u8"VsCode打开");
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
        qDebug() << "未选择按钮.";
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
            qDebug() << "按钮索引没有内容: " << buttonIndex;
        }
    }
    else {
        qDebug() << "未选择按钮.";
    }
}


void ButtonHandler::handlePrint3()
{
    if (currentButton) {
        qDebug() << currentButton->text();
    }
    else {
        qDebug() << "未选择按钮.";
    }
    if (currentButton) {
        int buttonIndex = buttons.indexOf(currentButton) + 1;
        QString filename = QString("%1/content_%2.lua").arg(currentConfigPath).arg(buttonIndex);


        qDebug() << u8"打开路径: " << filename;
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
        qDebug() << "未选择按钮.";
    }
}


void ButtonHandler::loadConfiguration(const QString& configName)
{
    currentConfigPath = QString("Config/%1").arg(configName);
    QDir dir(currentConfigPath);
    if (!dir.exists()) {
        qDebug() << "配置目录不存在: " << currentConfigPath;
        if (!dir.mkpath(currentConfigPath)) {
            qDebug() << "创建配置目录失败: " << currentConfigPath;
            return;
        }
    }

    // 初始化 contentMap
    contentMap.clear();
    for (int i = 1; i <= buttons.size(); ++i) {
        contentMap[i] = "";
    }

    // 加载按钮名称
    loadButtonNames();

    // 加载每个按钮的内容
    loadContentFromFile();

    // 如果有按钮，默认加载第一个按钮的内容并设置样式
    if (!buttons.isEmpty()) {
        loadContent(1);
        updateButtonStyles(1);
    }
}
