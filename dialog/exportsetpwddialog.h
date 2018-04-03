// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef EXPORTSETPWDDIALOG_H
#define EXPORTSETPWDDIALOG_H

#include <QDialog>

namespace Ui {
class ExportSetPwdDialog;
}

class ExportSetPwdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportSetPwdDialog(QWidget *parent = 0);
    ~ExportSetPwdDialog();

    bool  pop();

    QString pwd;

private slots:
    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_pkPwdLineEdit_textEdited(const QString &arg1);

    void on_confirmPwdLineEdit_textEdited(const QString &arg1);

private:
    Ui::ExportSetPwdDialog *ui;
    bool yesOrNO;

    void checkOkBtn();
};

#endif // EXPORTSETPWDDIALOG_H
