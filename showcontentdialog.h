// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef SHOWCONTENTDIALOG_H
#define SHOWCONTENTDIALOG_H

#include <QDialog>

namespace Ui {
class ShowContentDialog;
}

class ShowContentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShowContentDialog(QString text,QWidget *parent = 0);
    ~ShowContentDialog();

private slots:
    void on_copyBtn_clicked();

private:
    Ui::ShowContentDialog *ui;

};

#endif // SHOWCONTENTDIALOG_H
