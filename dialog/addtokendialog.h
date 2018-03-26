#ifndef ADDTOKENDIALOG_H
#define ADDTOKENDIALOG_H

#include <QDialog>

namespace Ui {
class AddTokenDialog;
}

class AddTokenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTokenDialog(QWidget *parent = 0);
    ~AddTokenDialog();

    void  pop();

private slots:
    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_contractAddressLineEdit_returnPressed();
private:
    Ui::AddTokenDialog *ui;
};

#endif // ADDTOKENDIALOG_H
