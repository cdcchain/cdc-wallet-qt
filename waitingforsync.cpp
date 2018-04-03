// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "waitingforsync.h"
#include "ui_waitingforsync.h"
#include "blockchain.h"
#include "debug_log.h"
#include "rpcthread.h"
#include "commondialog.h"
#include <QTimer>
#include <QDebug>
#include <QMovie>
#include <QDesktopServices>

WaitingForSync::WaitingForSync(QWidget *parent) :
    QWidget(parent),
    updateOrNot(false),
    synced(false),
    ui(new Ui::WaitingForSync)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/pic/cplpic/bg.png")));
    setPalette(palette);

    timer = new QTimer(this);    
    connect(timer,SIGNAL(timeout()),this,SLOT(updateInfo()));
    timer->start(5000);

    connect(CDC::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(infoUpdated(QString)));

    gif = new QMovie(":/pic/pic2/loading.gif");
    ui->gifLabel->setMovie(gif);
    gif->start();

    ui->minBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/minimize2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                              "QToolButton:hover{background-image:url(:/pic/pic2/minimize_hover.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/close2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                "QToolButton:hover{background-image:url(:/pic/pic2/close_hover.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");



    DLOG_QT_WALLET_FUNCTION_END;
}

WaitingForSync::~WaitingForSync()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    delete ui;
    DLOG_QT_WALLET_FUNCTION_END;
}

void WaitingForSync::updateInfo()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    RpcThread* rpcThread = new RpcThread;
    connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
    rpcThread->setWriteData( toJsonFormat( "id_info", "info", QStringList() << ""));
    rpcThread->start();

    updateBebuildInfo();

    DLOG_QT_WALLET_FUNCTION_END;
}

void WaitingForSync::on_minBtn_clicked()
{
    if( CDC::getInstance()->minimizeToTray)
    {
        emit tray();
    }
    else
    {
        emit minimum();
    }
}

void WaitingForSync::on_closeBtn_clicked()
{
    if( CDC::getInstance()->closeToMinimize)
    {
        emit tray();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText(tr( "Sure to close CDC Wallet?"));
        bool choice = commonDialog.pop();

        if( choice)
        {
            emit closeWallet();
        }
        else
        {
            return;
        }

    }
}

void WaitingForSync::infoUpdated(QString id)
{
    if( id != "id_info")  return;

    QString result = CDC::getInstance()->jsonDataValue( id);

    int pos = result.indexOf( "\"blockchain_head_block_age\":") + 28;
    QString seconds = result.mid( pos, result.indexOf("\"blockchain_head_block_timestamp\":") - pos - 1);
    qDebug() << "info seconds : " << seconds;
    qDebug() << "updateOrNot : "  << updateOrNot;
    // 不管是否连上过节点 直接进入
    if( /* seconds.toInt() != 0 && */ !updateOrNot && !synced)
    {
        emit sync();
        synced = true;
        return;
    }

    return;
}

// 比较版本号 若 a > b返回 1, a = b返回 0, a < b 返回 -1
int compareVersion( QString a, QString b)
{
    if( a == b)  return 0;

    QStringList aList = a.split(".");
    QStringList bList = b.split(".");

    if( aList.at(0).toInt() > bList.at(0).toInt() )
    {
        return 1;
    }
    else if( aList.at(0) < bList.at(0))
    {
        return -1;
    }
    else
    {
        if( aList.at(1).toInt() > bList.at(1).toInt() )
        {
            return 1;
        }
        else if( aList.at(1) < bList.at(1))
        {
            return -1;
        }
        else
        {
            if( aList.at(2).toInt() > bList.at(2).toInt() )
            {
                return 1;
            }
            else if( aList.at(2) < bList.at(2))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
    }
}


void WaitingForSync::updateBebuildInfo()
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QString str = gbkCodec->toUnicode(CDC::getInstance()->proc->readAll());
    qDebug() <<  "updateBebuildInfo " << str;
    if( str.contains("Replaying blockchain... Approximately ") && !str.contains("Successfully replayed"))
    {
        QStringList strList  = str.split("Replaying blockchain... Approximately ");
        QString percent = strList.last();
        percent = percent.mid(0, percent.indexOf("complete."));
        qDebug() << percent;
        ui->loadingLabel->setText( tr("Rebuilding... ") + percent );
    }

}


