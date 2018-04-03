// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef WAITINGFORSYNC_H
#define WAITINGFORSYNC_H

#include <QWidget>
#include <QtNetwork>
#include <QDomDocument>

namespace Ui {
class WaitingForSync;
}

class QNetworkReply;

class WaitingForSync : public QWidget
{
    Q_OBJECT

public:
    explicit WaitingForSync(QWidget *parent = 0);
    ~WaitingForSync();

    QTimer* timer;

signals:
    void sync();
    void minimum();
    void tray();
    void closeWallet();
    void showShadowWidget();
    void hideShadowWidget();

private slots:
    void updateInfo();

    void on_minBtn_clicked();

    void on_closeBtn_clicked();

//    void on_progressBar_valueChanged(int value);

    void infoUpdated(QString id);

//    void hasSomethingToRead();

private:
    Ui::WaitingForSync *ui;
    QMovie* gif;
    bool updateOrNot;
    bool synced;  // 防止发出两次sync()信号

    void updateBebuildInfo();

};

#endif // WAITINGFORSYNC_H
