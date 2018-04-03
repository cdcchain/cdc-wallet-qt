// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef REMARKDIALOG_H
#define REMARKDIALOG_H

#include <QDialog>

namespace Ui {
class RemarkDialog;
}

class RemarkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemarkDialog(QString address,QWidget *parent = 0);
    ~RemarkDialog();

    void pop();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_remarkLineEdit_returnPressed();

    void on_remarkLineEdit_textChanged(const QString &arg1);

private:
    Ui::RemarkDialog *ui;
    QString name;
};

#endif // REMARKDIALOG_H
