#ifndef TAB2WIDGET_H
#define TAB2WIDGET_H

#include <QWidget>
#include <QPushButton>
#include "AsdEngine.h"  // ÒýÈë MainWindow
class Tab2Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Tab2Widget(QWidget* parent, Ui::AsdEngineClass ui);
;
private slots:
    void on_button_clicked();
private:
    void showList(QMap<QString, QString> list);
    QPushButton* m_button;
    Ui::AsdEngineClass UI;
    std::map<int64_t, std::string> intToString;
};

#endif // TAB2WIDGET_H
