// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class TitleBar;
}

int applyEnable();    //  返回可以申请代理的账户个数

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = 0);
    ~TitleBar();
    void retranslator();

signals:
    void minimum();
    void closeWallet();
    void tray();
    void settingSaved();
    void showShadowWidget();
    void hideShadowWidget();
    void showAccountPage(QString);
    void lock();

private slots:
    void on_minBtn_clicked();

    void on_closeBtn_clicked();

    void on_menuBtn_clicked();

    void saved();

    void jsonDataUpdated(QString id);

    void onTimeOut();

    void on_consoleBtn_clicked();

    void on_lockBtn_clicked();

private:
    Ui::TitleBar *ui;
    QTimer* timer;

    void paintEvent(QPaintEvent*);
};

#endif // TITLEBAR_H
