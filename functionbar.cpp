// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "functionbar.h"
#include "ui_functionbar.h"
#include "blockchain.h"
#include "commondialog.h"
#include "debug_log.h"
#include "pubic_define.h"

#include <QPainter>
#include <QDebug>

QString getBtnStyle(bool isSelected, int pageIdx)
{
    QString style;
    QString icon;
    QString padding = "0px 15px 0px 0px";
    switch (pageIdx)
    {
    case 1:
        icon = "/pic/cplpic/accountIcon.png";
        break;
    case 2:
        icon = "/pic/cplpic/assetIcon.png";
        break;
    case 3:
        icon = "/pic/cplpic/transferIcon.png";
        break;
    case 4:
        icon = "/pic/cplpic/send.png";
        break;
    case 5:
        icon = "/pic/cplpic/contactIcon.png";
        padding = "0px 5px 0px 0px";
        break;
    }
    if (isSelected)
    {
        style.append("background-color: rgb("STR_MENU_BACKGROUND_COLOR");color: rgb(")
             .append(STR_BUTTON_COLOR");border:none;background-image: url(:")
             .append(icon).append(");background-repeat: no-repeat;background-position: left;")
             .append("background-attachment: fixed;background-clip: padding;border-style: flat;")
             .append("margin:0px 0px 0px 10px;padding:").append(padding).append(";");
    }
    else
    {
        style.append("background-color: rgb("STR_MENU_BACKGROUND_COLOR");color: rgb(255, 255, 255);")
             .append("border:none;background-image: url(:").append(icon)
             .append(");background-repeat: no-repeat;background-position: left;")
             .append("background-attachment: fixed;background-clip: padding;border-style: flat;")
             .append("margin:0px 0px 0px 10px;padding:").append(padding).append(";");
    }
    return style;
}


FunctionBar::FunctionBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionBar)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(MENU_BACKGROUND_COLOR));
    setPalette(palette);

    ui->multiSigBtn->hide();        // 多重签名 TODOTOMORROW
    ui->contactBtn->move(0,173);
    ui->transferBtn->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);

    choosePage(1);

    DLOG_QT_WALLET_FUNCTION_END;
}

FunctionBar::~FunctionBar()
{
    delete ui;
}

void FunctionBar::on_accountBtn_clicked()
{
    choosePage(1);
    emit showMainPage();
}

void FunctionBar::on_assetBtn_clicked()
{
    choosePage(2);
    emit showAssetPage();
}

void FunctionBar::on_transferBtn_clicked()
{
    mutexForAddressMap.lock();
    int size = CDC::getInstance()->addressMap.size();
    mutexForAddressMap.unlock();
    if( size > 0)   // 有至少一个账户
    {
        choosePage(3);
        emit showTransferPage();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkOnly);
        commonDialog.setText(tr("No account for transfering,\nadd an account first"));
        commonDialog.pop();
        on_accountBtn_clicked();
    }
}

void FunctionBar::on_multiSigBtn_clicked()
{
    choosePage(4);
    emit showMultiSigPage();
}

//void FunctionBar::on_marketBtn_clicked()
//{
//    choosePage(4);
//    emit showMarketPage();
//}

void FunctionBar::on_contactBtn_clicked()
{
    choosePage(5);
    emit showContactPage();
}

void FunctionBar::choosePage(int pageIndex)
{

    switch (pageIndex) {
    case 1:
        ui->accountBtn->setStyleSheet(getBtnStyle(true, 1));
        ui->assetBtn->setStyleSheet(getBtnStyle(false, 2));
        ui->transferBtn->setStyleSheet(getBtnStyle(false, 3));
        ui->multiSigBtn->setStyleSheet(getBtnStyle(false, 4));
        ui->contactBtn->setStyleSheet(getBtnStyle(false, 5));

        break;
    case 2:
        ui->accountBtn->setStyleSheet(getBtnStyle(false, 1));
        ui->assetBtn->setStyleSheet(getBtnStyle(true, 2));
        ui->transferBtn->setStyleSheet(getBtnStyle(false, 3));
        ui->multiSigBtn->setStyleSheet(getBtnStyle(false, 4));
        ui->contactBtn->setStyleSheet(getBtnStyle(false, 5));

        break;
    case 3:
        ui->accountBtn->setStyleSheet(getBtnStyle(false, 1));
        ui->assetBtn->setStyleSheet(getBtnStyle(false, 2));
        ui->transferBtn->setStyleSheet(getBtnStyle(true, 3));
        ui->multiSigBtn->setStyleSheet(getBtnStyle(false, 4));
        ui->contactBtn->setStyleSheet(getBtnStyle(false, 5));

        break;
    case 4:
        ui->accountBtn->setStyleSheet(getBtnStyle(false, 1));
        ui->assetBtn->setStyleSheet(getBtnStyle(false, 2));
        ui->transferBtn->setStyleSheet(getBtnStyle(false, 3));
        ui->multiSigBtn->setStyleSheet(getBtnStyle(true, 4));
        ui->contactBtn->setStyleSheet(getBtnStyle(false, 5));

        break;
    case 5:
        ui->accountBtn->setStyleSheet(getBtnStyle(false, 1));
        ui->assetBtn->setStyleSheet(getBtnStyle(false, 2));
        ui->transferBtn->setStyleSheet(getBtnStyle(false, 3));
        ui->multiSigBtn->setStyleSheet(getBtnStyle(false, 4));
        ui->contactBtn->setStyleSheet(getBtnStyle(true, 5));

        break;
    default:
        break;
    }
}

void FunctionBar::retranslator()
{
    ui->retranslateUi(this);
}

