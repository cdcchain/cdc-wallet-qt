// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <QDialog>

namespace Ui {
class AddContactDialog;
}

class AddContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddContactDialog(QWidget *parent = 0);
    ~AddContactDialog();

    void  pop();

private slots:
    void on_cancelBtn_clicked();

    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_remarkLineEdit_textChanged(const QString &arg1);

    void on_addressLineEdit_textEdited(const QString &arg1);

private:
    Ui::AddContactDialog *ui;
    QMovie* gif;

    bool eventFilter(QObject *watched, QEvent *e);
};

#endif // ADDCONTACTDIALOG_H
