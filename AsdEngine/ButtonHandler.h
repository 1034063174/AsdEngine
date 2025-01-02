#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <QObject>
#include <QList>
#include <QPushButton>
#include <Qsci/qsciscintilla.h>
#include <QSettings>
#include <QMap>
#include <QString>
#include <QComboBox>

// 前向声明 AsdEngine 类
class AsdEngine;

class ButtonHandler : public QObject
{
    Q_OBJECT

public:
    ButtonHandler(QsciScintilla* scintilla, QList<QPushButton*> buttons, QStringList buttonNames, QComboBox* comboBox, AsdEngine* engine, QObject* parent = nullptr);

    void saveCurrentContent();
    void loadContent(int buttonIndex);
    void updateButtonStyles(int activeButtonIndex);
    void saveButtonNames();
    void loadButtonNames();
    void loadConfiguration(const QString& configName);  // 新增：加载配置
public slots:
    void buttonclicked(int buttonIndex);

private slots:
    void showContextMenu(const QPoint& pos);
    void handlePrint1();
    void handlePrint2();
    void handlePrint3();
protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void saveContentToFile(int buttonIndex);
    void loadContentFromFile();

    QsciScintilla* scintilla;
    int currentButtonIndex;
    QList<QPushButton*> buttons;
    QStringList buttonNames;
    QMap<int, QString> contentMap;
    QSettings settings;
    QPushButton* currentButton;  // 新增：当前按钮
    AsdEngine* engine;  // 新增：指向 AsdEngine 的指针
    QComboBox* comboBox;  // 新增：指向 QComboBox 的指针
    QString currentConfigPath;  // 新增：当前配置路径
};

#endif // BUTTONHANDLER_H
