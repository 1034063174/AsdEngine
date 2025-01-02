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

// ǰ������ AsdEngine ��
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
    void loadConfiguration(const QString& configName);  // ��������������
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
    QPushButton* currentButton;  // ��������ǰ��ť
    AsdEngine* engine;  // ������ָ�� AsdEngine ��ָ��
    QComboBox* comboBox;  // ������ָ�� QComboBox ��ָ��
    QString currentConfigPath;  // ��������ǰ����·��
};

#endif // BUTTONHANDLER_H
