// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


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
