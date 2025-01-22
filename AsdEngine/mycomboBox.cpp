#include "mycombobox.h"

#include <QMouseEvent>


MyComboBox::MyComboBox(QWidget* parent) : QComboBox(parent)
{

}

void MyComboBox::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {//鼠标左键被按下
        emit on_ComboBox_clicked();
    }
    QComboBox::mousePressEvent(e);//将信号传递下去,
}