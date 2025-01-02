#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <QObject>
#include "ButtonHandler.h"
#include <Qsci/qsciscintilla.h>
#include <QList>
#include <QMap>
#include <QComboBox>

class ButtonManager : public QObject
{
    Q_OBJECT

public:
    ButtonManager(QsciScintilla* scintilla, QList<QPushButton*> buttons, QStringList buttonNames, QComboBox* comboBox, AsdEngine* engine, QObject* parent = nullptr);
    ~ButtonManager();
    ButtonHandler* buttonHandler;
};

#endif // BUTTONMANAGER_H
