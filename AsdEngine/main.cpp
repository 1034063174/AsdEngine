#include "AsdEngine.h"
#include <QtWidgets/QApplication>
#include "log.h"

int main(int argc, char *argv[])
{
 //   aaa();
    QApplication a(argc, argv);
    AsdEngine w;

    w.setStyleSheet(
        // 设置整个界面的样式
        "QWidget {"
        "    background-color: #2E2E2E;"    // 深灰色背景
        "    color: white;"                  // 白色文字
        "}"
        "QTabWidget {"
        "    background-color: #333333;"    // 深灰色背景
        "    color: white;"
        "}"
        "QTabWidget::pane {"
        "    background: #333333;"           // 选项卡面板背景深灰色
        "    border: 1px solid #555555;"     // 深灰色边框
        "}"
        "QTabBar::tab {"
        "    background-color: #444444;"     // 选项卡默认背景深灰色
        "    color: white;"
        "    border: 1px solid #555555;"     // 选项卡边框
        "}"
        "QTabBar::tab:selected {"
        "    background-color: #555555;"     // 选中标签的背景颜色稍微变亮
        "    color: white;"
        "}"
        "QTabBar::tab:hover {"
        "    background-color: #666666;"     // 悬停时的背景颜色略微亮一点
        "}"

        // 设置 QTableWidget 的样式
        "QTableWidget {"
        "    background-color: #2E2E2E;"    // 表格背景色
        "    color: white;"                  // 文字颜色
        "    gridline-color: #555555;"       // 网格线颜色
        "    border: 1px solid #555555;"     // 表格边框
        "}"
        "QTableWidget::item {"
        "    background-color: #444444;"     // 每个单元格背景颜色
        "    color: white;"                  // 每个单元格文字颜色
        "    border: 1px solid #555555;"     // 单元格边框
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #555555;"     // 选中单元格背景颜色
        "    color: white;"                  // 选中单元格文字颜色
        "}"
        "QTableWidget::horizontalHeader {"
        "    background-color: #333333;"     // 水平表头背景
        "    color: white;"                  // 水平表头文字颜色
        "    border: 1px solid #555555;"     // 水平表头边框
        "}"
        "QTableWidget::verticalHeader {"
        "    background-color: #333333;"     // 垂直表头背景
        "    color: white;"                  // 垂直表头文字颜色
        "    border: 1px solid #555555;"     // 垂直表头边框
        "}"
        "QHeaderView::section {"
        "    background-color: #444444;"     // 表头每一栏的背景颜色
        "    color: white;"                  // 表头每一栏的文字颜色
        "    border: 1px solid #555555;"     // 表头栏边框
        "}"

        // 设置 QLineEdit 的样式
        "QLineEdit {"
        "    background-color: #444444;"    // 深灰色背景
        "    color: white;"                  // 白色文字
        "    border: 1px solid #555555;"     // 边框颜色
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #666666;"     // 获取焦点时的边框颜色
        "    background-color: #555555;"    // 获取焦点时的背景色
        "}"
    );


    w.show();
    return a.exec();
}
