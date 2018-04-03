// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog( QString name, QWidget *parent = 0);
    ~ExportDialog();

    void  pop();

private slots:
    void on_pathBtn_clicked();

    void on_cancelBtn_clicked();

    void on_exportBtn_clicked();

    void jsonDataUpdated(QString id);

private:
    Ui::ExportDialog *ui;
    QString accoutName;
    QString privateKey;
    QString pwd;

    void getPrivateKey();
};

#endif // EXPORTDIALOG_H
