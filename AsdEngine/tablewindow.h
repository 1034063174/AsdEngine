#pragma once
#ifndef TABLEWINDOW_H
#define TABLEWINDOW_H

#include <QWidget>
#include <QTableWidget>

class TableWindow : public QWidget {
    Q_OBJECT

public:
    TableWindow(QWidget* parent = nullptr);
    void setTableContent( const QStringList& headers, const QList<QStringList>& items);

private:
    QTableWidget* tableWidget;
};

#endif // TABLEWINDOW_H
