#include "ButtonManager.h"

ButtonManager::ButtonManager(QsciScintilla* scintilla, QList<QPushButton*> buttons, QStringList buttonNames, QComboBox* comboBox, AsdEngine* engine, QObject* parent)
    : QObject(parent)
{
    buttonHandler = new ButtonHandler(scintilla, buttons, buttonNames, comboBox, engine, this);

    for (int i = 0; i < buttons.size(); ++i) {
        connect(buttons[i], &QPushButton::clicked, [=]() { buttonHandler->buttonclicked(i + 1); });
    }

 //   buttonHandler->buttonclicked(1); // ��ʼ��Ϊ��һ����ť
}

ButtonManager::~ButtonManager()
{
    delete buttonHandler;
}
