#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include "ui_UI_filter.h" // ȷ�������Զ����ɵ�ͷ�ļ�
#include <QComboBox>
class FilterDialog : public QDialog {
    Q_OBJECT

public:
    explicit FilterDialog(QWidget* parent = nullptr);
    ~FilterDialog();
    void saveRulFile(QString name);
    void LoadRulFile(QString name);
  
private slots:
    void on_pushButtonCreate_clicked();
    void initializeComboBox();
    void saveContentToFile();
    void ChooseFilterFile(const QString &name);
private:
    Ui::Dialog* ui;  // ʹ�� Ui::Dialog�������� Ui::Ui_Dialog
    QString currentConfigPath;  // ��������ǰ����·��
    QString filterString;
};

#endif // FILTERDIALOG_H
