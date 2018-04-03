// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef CHOOSEUPGRADEDIALOG_H
#define CHOOSEUPGRADEDIALOG_H

#include <QDialog>

namespace Ui {
class ChooseUpgradeDialog;
}

class ChooseUpgradeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseUpgradeDialog(QString name,QWidget *parent = 0);
    ~ChooseUpgradeDialog();

private slots:
    void on_applyDelegateBtn_clicked();

    void on_upgradeBtn_clicked();

signals:
    void upgrade(QString);
    void applyDelegate(QString);

private:
    Ui::ChooseUpgradeDialog *ui;
    QString accountName;

    bool event(QEvent *event);
};

#endif // CHOOSEUPGRADEDIALOG_H
