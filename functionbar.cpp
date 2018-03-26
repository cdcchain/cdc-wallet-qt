#include "functionbar.h"
#include "ui_functionbar.h"
#include "blockchain.h"
#include "commondialog.h"
#include "debug_log.h"

#include <QPainter>
#include <QDebug>

#define FUNCTIONPAGE_BTN_SELECTED       "background-color: rgb(125,85,40);color: rgb(255, 255, 255);border:none;"   // #7d5528
#define FUNCTIONPAGE_BTN_UNSELECTED     "background-color: rgb(185,150,85);color: rgb(255, 255, 255);border:none;"

FunctionBar::FunctionBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionBar)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(220,195,145));
    setPalette(palette);

    ui->multiSigBtn->hide();        // 多重签名 TODOTOMORROW
    ui->contactBtn->move(0,173);

//    ui->accountBtn->setIconSize(QSize(103,94));
//    ui->assetBtn->setIconSize(QSize(103,94));
//    ui->transferBtn->setIconSize(QSize(103,94));
////    ui->marketBtn->setIconSize(QSize(103,94));
//    ui->contactBtn->setIconSize(QSize(103,94));

//    ui->accountBtn->setStyleSheet("background:transparent;border:none;");
//    ui->assetBtn->setStyleSheet("background:transparent;border:none;");
//    ui->transferBtn->setStyleSheet("background:transparent;border:none;");
////    ui->marketBtn->setStyleSheet("background:transparent;border:none;");
//    ui->contactBtn->setStyleSheet("background:transparent;border:none;");


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
//        commonDialog.move( this->mapToGlobal(QPoint(318,150)));
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
//        ui->accountBtn->setIcon(QIcon(":/pic/cplpic/accountBtn.png"));

//        ui->assetBtn->setIcon(QIcon(":/pic/cplpic/assetBtn_unselected.png"));

//        ui->transferBtn->setIcon(QIcon(":/pic/cplpic/transferBtn_unselected.png"));

////        ui->marketBtn->setIcon(QIcon(":/pic/cplpic/marketBtn_unselected.png"));

//        ui->contactBtn->setIcon(QIcon(":/pic/cplpic/contactBtn_unselected.png"));

        ui->accountBtn->setStyleSheet(FUNCTIONPAGE_BTN_SELECTED);
        ui->assetBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->transferBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->multiSigBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->contactBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);

        break;
    case 2:
//        ui->accountBtn->setIcon(QIcon(":/pic/cplpic/accountBtn_unselected.png"));

//        ui->assetBtn->setIcon(QIcon(":/pic/cplpic/assetBtn.png"));

//        ui->transferBtn->setIcon(QIcon(":/pic/cplpic/transferBtn_unselected.png"));

////        ui->marketBtn->setIcon(QIcon(":/pic/cplpic/marketBtn_unselected.png"));

//        ui->contactBtn->setIcon(QIcon(":/pic/cplpic/contactBtn_unselected.png"));

        ui->accountBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->assetBtn->setStyleSheet(FUNCTIONPAGE_BTN_SELECTED);
        ui->transferBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->multiSigBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->contactBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);

        break;
    case 3:
//        ui->accountBtn->setIcon(QIcon(":/pic/cplpic/accountBtn_unselected.png"));

//        ui->assetBtn->setIcon(QIcon(":/pic/cplpic/assetBtn_unselected.png"));

//        ui->transferBtn->setIcon(QIcon(":/pic/cplpic/transferBtn.png"));

////        ui->marketBtn->setIcon(QIcon(":/pic/cplpic/marketBtn_unselected.png"));

//        ui->contactBtn->setIcon(QIcon(":/pic/cplpic/contactBtn_unselected.png"));

        ui->accountBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->assetBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->transferBtn->setStyleSheet(FUNCTIONPAGE_BTN_SELECTED);
        ui->multiSigBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->contactBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);

        break;
    case 4:
//        ui->accountBtn->setIcon(QIcon(":/pic/cplpic/accountBtn_unselected.png"));

//        ui->assetBtn->setIcon(QIcon(":/pic/cplpic/assetBtn_unselected.png"));

//        ui->transferBtn->setIcon(QIcon(":/pic/cplpic/transferBtn_unselected.png"));

////        ui->marketBtn->setIcon(QIcon(":/pic/cplpic/marketBtn.png"));

//        ui->contactBtn->setIcon(QIcon(":/pic/cplpic/contactBtn_unselected.png"));

        ui->accountBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->assetBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->transferBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->multiSigBtn->setStyleSheet(FUNCTIONPAGE_BTN_SELECTED);
        ui->contactBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);

        break;
    case 5:
//        ui->accountBtn->setIcon(QIcon(":/pic/cplpic/accountBtn_unselected.png"));

//        ui->assetBtn->setIcon(QIcon(":/pic/cplpic/assetBtn_unselected.png"));

//        ui->transferBtn->setIcon(QIcon(":/pic/cplpic/transferBtn_unselected.png"));

////        ui->marketBtn->setIcon(QIcon(":/pic/cplpic/marketBtn_unselected.png"));

//        ui->contactBtn->setIcon(QIcon(":/pic/cplpic/contactBtn.png"));

        ui->accountBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->assetBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->transferBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->multiSigBtn->setStyleSheet(FUNCTIONPAGE_BTN_UNSELECTED);
        ui->contactBtn->setStyleSheet(FUNCTIONPAGE_BTN_SELECTED);

        break;
    default:
        break;
    }
}

void FunctionBar::retranslator()
{
    ui->retranslateUi(this);
}

