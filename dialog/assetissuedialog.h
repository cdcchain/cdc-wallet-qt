// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef ASSETISSUEDIALOG_H
#define ASSETISSUEDIALOG_H

#include <QDialog>

namespace Ui {
class AssetIssueDialog;
}

class AssetIssueDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AssetIssueDialog( int assetId, QWidget *parent = 0);
    ~AssetIssueDialog();

    int id;

    void  pop();

private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void jsonDataUpdated( QString id);

    void on_amountLineEdit_textChanged(const QString &arg1);

private:
    Ui::AssetIssueDialog *ui;
};

#endif // ASSETISSUEDIALOG_H
