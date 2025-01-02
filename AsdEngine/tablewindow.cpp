#include "tablewindow.h"
#include <QVBoxLayout>

TableWindow::TableWindow(QWidget* parent)
    : QWidget(parent, Qt::Window)  // 设置为独立窗口
{
    tableWidget = new QTableWidget(this);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(tableWidget);
    setLayout(layout);
}

void TableWindow::setTableContent(const QStringList& headers, const QList<QStringList>& items) {
    tableWidget->setRowCount(items.size());
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);
    int columns = items[0].size();
    int rows = items.size();
    for (int row = 0; row < rows; ++row) {
        if (row < items.size()) {
            const QStringList& rowData = items[row];
            for (int column = 0; column < columns; ++column) {
                if (column < rowData.size()) {
                    tableWidget->setItem(row, column, new QTableWidgetItem(rowData[column]));
                }
            }
        }
    }
}
