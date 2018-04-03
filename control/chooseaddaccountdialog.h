// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef CHOOSEADDACCOUNTDIALOG_H
#define CHOOSEADDACCOUNTDIALOG_H

#include <QDialog>

namespace Ui {
class ChooseAddAccountDialog;
}

class ChooseAddAccountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseAddAccountDialog(QWidget *parent = 0);
    ~ChooseAddAccountDialog();

private slots:
    void on_newBtn_clicked();

    void on_importBtn_clicked();

signals:
    void newAccount();
    void importAccount();

private:
    Ui::ChooseAddAccountDialog *ui;

    bool event(QEvent *event);
};

#endif // CHOOSEADDACCOUNTDIALOG_H
