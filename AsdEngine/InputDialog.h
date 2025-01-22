#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class InputDialog : public QDialog {
    Q_OBJECT

public:
    InputDialog(QWidget* parent = nullptr) : QDialog(parent) {
        // 设置输入框
        m_lineEdit = new QLineEdit(this);

        // 设置按钮
        m_okButton = new QPushButton("确定", this);

        // 设置布局
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(m_lineEdit);
        layout->addWidget(m_okButton);

        connect(m_okButton, &QPushButton::clicked, this, &InputDialog::onOkClicked);
    }

    // 获取输入框的值
    QString getInputText() const {
        return m_lineEdit->text();
    }

private slots:
    void onOkClicked() {
        accept();  // 关闭对话框
    }

private:
    QLineEdit* m_lineEdit;
    QPushButton* m_okButton;
};
