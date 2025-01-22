#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include "ui_UI_filter.h" // 确保包含自动生成的头文件
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
    Ui::Dialog* ui;  // 使用 Ui::Dialog，而不是 Ui::Ui_Dialog
    QString currentConfigPath;  // 新增：当前配置路径
    QString filterString;
};

#endif // FILTERDIALOG_H
