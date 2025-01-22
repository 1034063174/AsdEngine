#include "AsdEngine.h"
#include <QtWidgets/QApplication>
#include "log.h"

int main(int argc, char *argv[])
{
 //   aaa();
    QApplication a(argc, argv);
    AsdEngine w;

    w.setStyleSheet(
        // ���������������ʽ
        "QWidget {"
        "    background-color: #2E2E2E;"    // ���ɫ����
        "    color: white;"                  // ��ɫ����
        "}"
        "QTabWidget {"
        "    background-color: #333333;"    // ���ɫ����
        "    color: white;"
        "}"
        "QTabWidget::pane {"
        "    background: #333333;"           // ѡ���屳�����ɫ
        "    border: 1px solid #555555;"     // ���ɫ�߿�
        "}"
        "QTabBar::tab {"
        "    background-color: #444444;"     // ѡ�Ĭ�ϱ������ɫ
        "    color: white;"
        "    border: 1px solid #555555;"     // ѡ��߿�
        "}"
        "QTabBar::tab:selected {"
        "    background-color: #555555;"     // ѡ�б�ǩ�ı�����ɫ��΢����
        "    color: white;"
        "}"
        "QTabBar::tab:hover {"
        "    background-color: #666666;"     // ��ͣʱ�ı�����ɫ��΢��һ��
        "}"

        // ���� QTableWidget ����ʽ
        "QTableWidget {"
        "    background-color: #2E2E2E;"    // ��񱳾�ɫ
        "    color: white;"                  // ������ɫ
        "    gridline-color: #555555;"       // ��������ɫ
        "    border: 1px solid #555555;"     // ���߿�
        "}"
        "QTableWidget::item {"
        "    background-color: #444444;"     // ÿ����Ԫ�񱳾���ɫ
        "    color: white;"                  // ÿ����Ԫ��������ɫ
        "    border: 1px solid #555555;"     // ��Ԫ��߿�
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #555555;"     // ѡ�е�Ԫ�񱳾���ɫ
        "    color: white;"                  // ѡ�е�Ԫ��������ɫ
        "}"
        "QTableWidget::horizontalHeader {"
        "    background-color: #333333;"     // ˮƽ��ͷ����
        "    color: white;"                  // ˮƽ��ͷ������ɫ
        "    border: 1px solid #555555;"     // ˮƽ��ͷ�߿�
        "}"
        "QTableWidget::verticalHeader {"
        "    background-color: #333333;"     // ��ֱ��ͷ����
        "    color: white;"                  // ��ֱ��ͷ������ɫ
        "    border: 1px solid #555555;"     // ��ֱ��ͷ�߿�
        "}"
        "QHeaderView::section {"
        "    background-color: #444444;"     // ��ͷÿһ���ı�����ɫ
        "    color: white;"                  // ��ͷÿһ����������ɫ
        "    border: 1px solid #555555;"     // ��ͷ���߿�
        "}"

        // ���� QLineEdit ����ʽ
        "QLineEdit {"
        "    background-color: #444444;"    // ���ɫ����
        "    color: white;"                  // ��ɫ����
        "    border: 1px solid #555555;"     // �߿���ɫ
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #666666;"     // ��ȡ����ʱ�ı߿���ɫ
        "    background-color: #555555;"    // ��ȡ����ʱ�ı���ɫ
        "}"
    );


    w.show();
    return a.exec();
}
