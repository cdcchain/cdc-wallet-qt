// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <QDebug>
#include <QPainter>

#include "assetpage.h"
#include "ui_assetpage.h"
#include "pubic_define.h"
#include "blockchain.h"
#include "dialog/assetissuedialog.h"
#include "commondialog.h"
#include "dialog/addtokendialog.h"
#include "showcontentdialog.h"


AssetPage::AssetPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssetPage)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(BACKGROUND_COLOR));
    setPalette(palette);

    connect( CDC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    updateAssetInfo();
    updateMyAsset();

    ui->myAssetWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->myAssetWidget->setColumnWidth(0,80);
    ui->myAssetWidget->setColumnWidth(1,80);
    ui->myAssetWidget->setColumnWidth(2,290);
    ui->myAssetWidget->setColumnWidth(3,120);
    ui->myAssetWidget->setColumnWidth(4,100);
    ui->myAssetWidget->setColumnWidth(5,0);
    ui->myAssetWidget->setShowGrid(true);
    ui->myAssetWidget->setFrameShape(QFrame::NoFrame);
    ui->myAssetWidget->setMouseTracking(true);
    ui->myAssetWidget->horizontalHeader()->setStyleSheet("QHeaderView:section{color:white;background-color:rgb("
                                                         STR_MENU_BACKGROUND_COLOR");border:1px solid grey;}");
    ui->myAssetWidget->horizontalHeader()->setVisible(true);
    ui->addTokenBtn->setStyleSheet(QTOOL_BUTTON_STYLE);
}

AssetPage::~AssetPage()
{
    qDebug() << __FUNCTION__;
    delete ui;
}

void AssetPage::updateAssetInfo()
{
}

void AssetPage::updateMyAsset()
{
    ui->myAssetWidget->setRowCount(0);
    ui->myAssetWidget->setRowCount(CDC::getInstance()->ERC20TokenInfoMap.size());

    if(CDC::getInstance()->ERC20TokenInfoMap.keys().size() > 0)
    {
        ui->myAssetWidget->setColumnWidth(5,80);
    }
    else
    {
        ui->myAssetWidget->setColumnWidth(5,0);
    }

    int myAssetCount = 0;
    foreach (QString contractAddress, CDC::getInstance()->ERC20TokenInfoMap.keys())
    {
        ui->myAssetWidget->setRowHeight(myAssetCount,45);

        ERC20TokenInfo info = CDC::getInstance()->ERC20TokenInfoMap.value(contractAddress);
//        qDebug() << __FUNCTION__" " << info.name << ", " << info.symbol << ", " << info.contractAddress;
        ui->myAssetWidget->setItem(myAssetCount,0,new QTableWidgetItem(info.name));
        ui->myAssetWidget->setItem(myAssetCount,1,new QTableWidgetItem(info.symbol));
        ui->myAssetWidget->setItem(myAssetCount,2,new QTableWidgetItem(info.contractAddress));
        ui->myAssetWidget->setItem(myAssetCount,3,new QTableWidgetItem(getBigNumberString(info.totalSupply,info.precision)));
        ui->myAssetWidget->setItem(myAssetCount,4,new QTableWidgetItem(getBigNumberString(info.precision,1)));
//        ui->myAssetWidget->setItem(myAssetCount,5,new QTableWidgetItem(tr("unfollow")));
//        ui->myAssetWidget->item(myAssetCount,5)->setTextColor(QColor(65,205,82));

//        for(int j = 0; j < 6; j++)
//        {
//            ui->myAssetWidget->item(myAssetCount,j)->setTextAlignment(Qt::AlignCenter);
//        }
//        qDebug() << __FUNCTION__" " << myAssetCount;
        myAssetCount++;
    }

    ui->myAssetWidget->setRowCount(myAssetCount);
}

void AssetPage::jsonDataUpdated(QString id)
{
}

void AssetPage::on_myAssetWidget_cellClicked(int row, int column)
{
    if( column == 5)
    {
        QString symbol = ui->myAssetWidget->item(row,1)->text();
        QString contractAddress = ui->myAssetWidget->item(row,2)->text();

        CommonDialog tipDialog(CommonDialog::YesOrNo);
        tipDialog.setText( tr("Sure to unfollow ") + symbol + " ?");

        if( tipDialog.pop() )
        {
            CDC::getInstance()->ERC20TokenInfoMap.remove(contractAddress);
            CDC::getInstance()->configFile->remove("/AddedContractToken/" + contractAddress);

            foreach (QString key, CDC::getInstance()->accountContractTransactionMap.keys())
            {
                if( key.contains(contractAddress))
                {
                    CDC::getInstance()->accountContractTransactionMap.remove(key);
                }
            }

            updateMyAsset();
        }
    }
}

void AssetPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(228,228,228),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(TITLE_COLOR),Qt::SolidPattern));
    painter.drawRect(-1,-1,858,66);
}

void AssetPage::on_addTokenBtn_clicked()
{
    AddTokenDialog addTokenDialog;
    addTokenDialog.pop();
}

void AssetPage::on_myAssetWidget_cellPressed(int row, int column)
{
    if( column == 2 )
    {
        ShowContentDialog showContentDialog( ui->myAssetWidget->item(row, column)->text(),this);

        int x = ui->myAssetWidget->columnViewportPosition(column) + ui->myAssetWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->myAssetWidget->rowViewportPosition(row) - 10 + ui->myAssetWidget->horizontalHeader()->height();

        showContentDialog.move( ui->myAssetWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}
