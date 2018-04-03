// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "bottombar.h"
#include "ui_bottombar.h"
#include "consoledialog.h"
#include "blockchain.h"
#include "debug_log.h"
#include "commontip.h"
#include "rpcthread.h"
#include "commondialog.h"
#include "extra/dynamicmove.h"

#include <QPainter>
#include <QTimer>
#include <QMovie>
#include <QMouseEvent>

BottomBar::BottomBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BottomBar)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    ui->networkLabel->setPixmap(QPixmap(":/pic/cplpic/network.png"));
    ui->syncLabel->setStyleSheet("color:rgb(112,104,103)");
    ui->nodeNumLabel->setStyleSheet("color:rgb(112,104,103)");
//    ui->syncLabel->setToolTip(tr("Local block height"));

    connect(CDC::getInstance(), SIGNAL(jsonDataUpdated(QString)),this, SLOT(jsonDataUpdated(QString)));
    jsonDataUpdated("id_info");

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(229,229,227));
    setPalette(palette);

    timer = new QTimer(this);
    timer->start(5000);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateNumOfConnections()));
    updateNumOfConnections();

    connectionTip = new CommonTip;


    DLOG_QT_WALLET_FUNCTION_END;
}

BottomBar::~BottomBar()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    delete ui;
    delete timer;
    DLOG_QT_WALLET_FUNCTION_END;
}



void BottomBar::updateNumOfConnections()
{
//    DLOG_QT_WALLET_FUNCTION_BEGIN;

    QString result = CDC::getInstance()->jsonDataValue("id_info");
    if( result.isEmpty())  return;

    int pos = result.indexOf("\"network_num_connections\"") + 26;
    QString num = result.mid( pos, result.indexOf("," , pos) - pos);

    ui->nodeNumLabel->setText(num + tr(" peers"));


//    DLOG_QT_WALLET_FUNCTION_END;
}


void BottomBar::retranslator()
{
    ui->retranslateUi(this);

//    ui->syncLabel->setToolTip(tr("Local block height"));

}

void BottomBar::jsonDataUpdated(QString id)
{
    if( id == "id_info")
    {
        CDC::getInstance()->parseBalance();

        QString result = CDC::getInstance()->jsonDataValue( id);
        if( result.isEmpty() )  return;

        int pos = result.indexOf( "\"blockchain_head_block_age\":") + 28;
        QString seconds = result.mid( pos, result.indexOf("\"blockchain_head_block_timestamp\":") - pos - 1);

        int pos2 = result.indexOf( "\"blockchain_head_block_num\":") + 28;
        QString num = result.mid( pos2, result.indexOf("\"blockchain_head_block_age\":") - pos2 - 1);

        if( seconds.toInt() < 0)  seconds = "0";

        ui->syncLabel->setText( tr("Local block height:  ") + num );
        CDC::getInstance()->currentBlockHeight = num.toInt();

        int pos3 = result.indexOf( "\"wallet_scan_progress\":") + 23;
        QString scanProgress = result.mid( pos3, result.indexOf(",\"wallet_block_production_enabled\":") - pos3);

        scanProgress.remove('\"');
        double scanPercent = scanProgress.toDouble();

        if( CDC::getInstance()->needToScan && scanPercent > 0.99999)
        {
            CDC::getInstance()->postRPC( toJsonFormat( "id_scan", "scan", QStringList() << "0"));
            CDC::getInstance()->needToScan = false;
        }

        return;
    }


}

void BottomBar::refresh()
{
    CDC::getInstance()->postRPC( toJsonFormat( "id_info", "info", QStringList() << ""));
    CDC::getInstance()->postRPC( toJsonFormat( "id_blockchain_list_assets", "blockchain_list_assets", QStringList() << ""));
}
