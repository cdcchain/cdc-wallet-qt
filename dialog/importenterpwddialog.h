// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef IMPORTENTERPWDDIALOG_H
#define IMPORTENTERPWDDIALOG_H

#include <QDialog>

namespace Ui {
class ImportEnterPwdDialog;
}

class ImportEnterPwdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportEnterPwdDialog(QWidget *parent = 0);
    ~ImportEnterPwdDialog();

    bool  pop();

    QString pwd;

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_pkPwdLineEdit_textEdited(const QString &arg1);

private:
    Ui::ImportEnterPwdDialog *ui;
    bool yesOrNO;

    void checkOkBtn();
};

#endif // IMPORTENTERPWDDIALOG_H
