#ifndef TRANSFERCONFIRMDIALOG_H
#define TRANSFERCONFIRMDIALOG_H

#include <QDialog>

namespace Ui {
class TransferConfirmDialog;
}

class TransferConfirmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransferConfirmDialog(QString address, QString amount, QString fee, QString remark, QString assetSymbol, QWidget *parent = 0);
    ~TransferConfirmDialog();

    bool pop();

private slots:
    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_pwdLineEdit_textChanged(const QString &arg1);

    void on_pwdLineEdit_returnPressed();

private:
    Ui::TransferConfirmDialog *ui;
    QString address;
    QString amount;
    QString fee;
    QString remark;
    bool yesOrNo;
};

#endif // TRANSFERCONFIRMDIALOG_H
