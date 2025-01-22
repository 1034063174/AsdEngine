#pragma once
#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H

#include <QComboBox>
#include <QWidget>

class MyComboBox : public QComboBox
{
    Q_OBJECT
public:
    MyComboBox(QWidget* parent);
protected:
    void mousePressEvent(QMouseEvent* e) override;
signals:
    void on_ComboBox_clicked();
};

#endif // MYCOMBOBOX_H