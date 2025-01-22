#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class InputDialog : public QDialog {
    Q_OBJECT

public:
    InputDialog(QWidget* parent = nullptr) : QDialog(parent) {
        // ���������
        m_lineEdit = new QLineEdit(this);

        // ���ð�ť
        m_okButton = new QPushButton("ȷ��", this);

        // ���ò���
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(m_lineEdit);
        layout->addWidget(m_okButton);

        connect(m_okButton, &QPushButton::clicked, this, &InputDialog::onOkClicked);
    }

    // ��ȡ������ֵ
    QString getInputText() const {
        return m_lineEdit->text();
    }

private slots:
    void onOkClicked() {
        accept();  // �رնԻ���
    }

private:
    QLineEdit* m_lineEdit;
    QPushButton* m_okButton;
};
