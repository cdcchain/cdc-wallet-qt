// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <QDebug>
#include <QPainter>
#include <QHelpEvent>
#include <QDateTime>
#include <QTextCodec>
#include <QScrollBar>
#include <string>
#include <QClipboard>
#include "pubic_define.h"
#include "accountpage.h"
#include "blockchain.h"
#include "ui_accountpage.h"
#include "debug_log.h"
#include "commondialog.h"
#include "showcontentdialog.h"
#include "rpcthread.h"
#include "control/remarkcellwidget.h"
#include "control/qrcodedialog.h"


AccountPage::AccountPage(QString name, QWidget *parent) :
    QWidget(parent),
    accountName(name),
    transactionType(0),
    address(""),
    inited(false),
    assetUpdating(false),
    ui(new Ui::AccountPage)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(TITLE_COLOR));
    setPalette(palette);

    connect( CDC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    mutexForAddressMap.lock();
    if( accountName.isEmpty())  // 如果是点击账单跳转
    {
        if( CDC::getInstance()->addressMap.size() > 0)
        {
            accountName = CDC::getInstance()->addressMap.keys().at(0);
        }
        else  // 如果还没有账户
        {
            mutexForAddressMap.unlock();
            emit back();
            return;
        }
    }

    // 账户下拉框按字母顺序排序
    QStringList keys = CDC::getInstance()->addressMap.keys();
    ui->accountComboBox->addItems( keys);
    if( accountName.isEmpty() )
    {
        ui->accountComboBox->setCurrentIndex(0);
    }
    else
    {
        ui->accountComboBox->setCurrentText( accountName);
    }
    mutexForAddressMap.unlock();

    ui->pageLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->pageLineEdit->setText("1");
    QIntValidator *validator = new QIntValidator(1,9999,this);
    ui->pageLineEdit->setValidator( validator );
    ui->pageLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    currentPageIndex = 1;
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    ui->prePageBtn->setStyleSheet("QToolButton:!hover{border:0px;color:#999999;} QToolButton:hover{border:0px;color:#7d5528;}");
    ui->nextPageBtn->setStyleSheet("QToolButton:!hover{border:0px;color:#999999;} QToolButton:hover{border:0px;color:#7d5528;}");

#ifdef WIN32
    ui->accountComboBox->setStyleSheet("QComboBox{border: 1px solid gray;border-radius: 3px;padding: 1px 2px 1px 8px;}"
                  "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 20px;"
                  "border-left-width: 1px;border-left-color: darkgray;border-left-style: solid;"
                  "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                  "QComboBox::down-arrow {image: url(:/pic/pic2/comboBoxArrow.png);}"
                  );

    ui->assetComboBox->setStyleSheet("QComboBox{border: 1px solid gray;border-radius: 3px;padding: 1px 2px 1px 8px;}"
                  "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 20px;"
                  "border-left-width: 1px;border-left-color: darkgray;border-left-style: solid;"
                  "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                  "QComboBox::down-arrow {image: url(:/pic/pic2/comboBoxArrow.png);}"
                  );
#endif

    ui->copyBtn->setStyleSheet("QToolButton{background-image:url(:/pic/cplpic/copy.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->copyBtn->setToolTip(tr("copy to clipboard"));

    ui->qrcodeBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/qrcode.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    ui->accountTransactionsTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->accountTransactionsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->accountTransactionsTableWidget->setColumnWidth(0,100);
    ui->accountTransactionsTableWidget->setColumnWidth(1,100);
    ui->accountTransactionsTableWidget->setColumnWidth(2,130);
    ui->accountTransactionsTableWidget->setColumnWidth(3,100);
    ui->accountTransactionsTableWidget->setColumnWidth(4,100);
    ui->accountTransactionsTableWidget->setColumnWidth(5,100);
    ui->accountTransactionsTableWidget->setColumnWidth(6,130);
    ui->accountTransactionsTableWidget->setShowGrid(true);
    ui->accountTransactionsTableWidget->setFrameShape(QFrame::NoFrame);
    ui->accountTransactionsTableWidget->setMouseTracking(true);
    ui->accountTransactionsTableWidget->horizontalHeader()->setVisible(true);
    ui->initLabel->hide();

    updateAssetList();

    if( !CDC::getInstance()->currentTokenAddress.isEmpty())
    {
        ui->assetComboBox->setCurrentText(CDC::getInstance()->ERC20TokenInfoMap.value(CDC::getInstance()->currentTokenAddress).symbol);
    }

    init();
    updateTransactionsList();

    inited = true;

    DLOG_QT_WALLET_FUNCTION_END;
}

QString discard(const QString &str)
{
    int dotPos = str.indexOf(".");
    if( dotPos != -1)
    {
        return str.left( dotPos + 3);
    }
    else
    {
		DLOG_QT_WALLET(" no dot!");
        return NULL;
    }
}

AccountPage::~AccountPage()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    delete ui;
    DLOG_QT_WALLET_FUNCTION_END;

}

void AccountPage::init()
{

    QString showName = CDC::getInstance()->addressMapValue(accountName).ownerAddress;
    address = showName;
    showName = showName.left(9) + "..." + showName.right(3);
    ui->addressLabel->setText( showName);
    ui->addressLabel->adjustSize();
    ui->addressLabel->setGeometry(ui->addressLabel->x(),ui->addressLabel->y(),ui->addressLabel->width(),24);
    ui->copyBtn->move(ui->addressLabel->x() + ui->addressLabel->width() + 9, 39);
    ui->qrcodeBtn->move(ui->addressLabel->x() + ui->addressLabel->width() + 30, 39);
}

void AccountPage::updateTransactionsList()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    if( ui->assetComboBox->currentIndex() == 0)
    {
        CDC::getInstance()->postRPC( toJsonFormat( "id_history_" + accountName + "_" + ui->assetComboBox->currentText(), "history", QStringList() << accountName << ui->assetComboBox->currentText() << "9999999" ));
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

void AccountPage::updateAssetList()
{
    assetUpdating = true;

    ui->assetComboBox->clear();
    ui->assetComboBox->addItem(ASSET_NAME);
    foreach (QString key, CDC::getInstance()->ERC20TokenInfoMap.keys())
    {
        qDebug() << __FUNCTION__ << ": " << key << ", " << CDC::getInstance()->ERC20TokenInfoMap.value(key).symbol;
        ui->assetComboBox->addItem( CDC::getInstance()->ERC20TokenInfoMap.value(key).symbol);
    }

    assetUpdating = false;
}

void AccountPage::updateBalance()
{
    if( ui->assetComboBox->currentIndex() == 0)
    {
        if( !CDC::getInstance()->accountBalanceMap.contains(accountName))
        {
            ui->balanceLabel->setText( "<body><font style=\"font-size:18px\" color=#000000>0</font><font style=\"font-size:12px\" color=#000000> " + ui->assetComboBox->currentText() + "</font></body>" );
            return;
        }

        AssetBalanceMap assetBalanceMap = CDC::getInstance()->accountBalanceMap.value(accountName);
        int assetId = CDC::getInstance()->getAssetId(ui->assetComboBox->currentText());
        if( assetId < 0)    return;
        unsigned long long amount = assetBalanceMap.value(assetId);
        AssetInfo info = CDC::getInstance()->assetInfoMap.value(assetId);
        ui->balanceLabel->setText( "<body><font style=\"font-size:18px\" color=#000000>" + getBigNumberString(amount,info.precision) + "</font><font style=\"font-size:12px\" color=#000000> " + ui->assetComboBox->currentText() + "</font></body>" );
        ui->balanceLabel->adjustSize();
    }
    else
    {
        // 如果是合约资产
        QStringList contracts = CDC::getInstance()->ERC20TokenInfoMap.keys();
        QString contractAddress = contracts.at(ui->assetComboBox->currentIndex() - 1);

        ERC20TokenInfo info = CDC::getInstance()->ERC20TokenInfoMap.value(contractAddress);

        QString accountAddress = CDC::getInstance()->addressMap.value(accountName).ownerAddress;
        unsigned long long amount = CDC::getInstance()->accountContractBalanceMap.value(accountAddress).value(contractAddress);
        ui->balanceLabel->setText( "<body><font style=\"font-size:18px\" color=#000000>" + getBigNumberString(amount,info.precision) + "</font><font style=\"font-size:12px\" color=#000000> " + ui->assetComboBox->currentText() + "</font></body>" );
        ui->balanceLabel->adjustSize();
    }

}


void AccountPage::refresh()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    init();
    updateTransactionsList();
    showTransactions();
    updateBalance();

    DLOG_QT_WALLET_FUNCTION_END;
}

void AccountPage::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(address);

    CommonDialog commonDialog(CommonDialog::OkOnly);
    commonDialog.setText(tr("Copy to clipboard"));
    commonDialog.pop();
}

void AccountPage::on_accountComboBox_currentIndexChanged(const QString &arg1)
{
    if( inited)  // 防止accountComboBox当前值改变的时候触发
    {
        accountName = arg1;
        CDC::getInstance()->currentAccount = accountName;
        currentPageIndex = 1;
        ui->pageLineEdit->setText( QString::number(currentPageIndex));
        init();
        updateTransactionsList();
        updateBalance();
        showTransactions();
    }
}


void AccountPage::jsonDataUpdated(QString id)
{
    if( id == "id_history_" + accountName + "_" + ui->assetComboBox->currentText())
    {
        QString result = CDC::getInstance()->jsonDataValue(id);
        CDC::getInstance()->parseTransactions(result,accountName + "_" + ui->assetComboBox->currentText());

        return;
    }
}



void AccountPage::showTransactions()
{
    if( ui->assetComboBox->currentIndex() == 0)
    {
        showNormalTransactions();
    }
    else
    {
        showContractTransactions();
    }
}

void AccountPage::showNormalTransactions()
{
    ui->accountTransactionsTableWidget->setColumnHidden(6,false);

    TransactionsInfoVector vector = CDC::getInstance()->transactionsMap.value(accountName + "_" + ui->assetComboBox->currentText());

    ui->accountTransactionsTableWidget->setRowCount(0);
    if( vector.size() < 1)
    {
        ui->prePageBtn->hide();
        ui->numberLabel->hide();
        ui->pageLineEdit->hide();
        ui->pageLabel->hide();
        ui->nextPageBtn->hide();
        ui->initLabel->show();
        return;
    }
    ui->prePageBtn->show();
    ui->numberLabel->show();
    ui->pageLineEdit->show();
    ui->pageLabel->show();
    ui->nextPageBtn->show();
    ui->initLabel->hide();

    int size = vector.size();
    ui->numberLabel->setText( tr("total ") + QString::number( size) + tr(" ,"));
    ui->pageLabel->setText( "/" + QString::number( (size - 1)/10 + 1 ) );

    int rowCount = size - (currentPageIndex - 1) * 10;
    if( rowCount > 10 )  rowCount = 10;  // 一页最多显示10行
    ui->accountTransactionsTableWidget->setRowCount(rowCount);

    for(int i = rowCount - 1; i > -1; i--)
    {
        ui->accountTransactionsTableWidget->setRowHeight(i,57);
        TransactionInfo transactionInfo = vector.at(  size - ( i + 1) - (currentPageIndex - 1) * 10 );

        // 时间戳
        QString date = transactionInfo.timeStamp;
        date.replace(QString("T"),QString(" "));
        QDateTime time = QDateTime::fromString(date, "yyyy-MM-dd hh:mm:ss");
        time = time.addSecs(28800);       // 时间加8小时
        QString currentDateTime = time.toString("yyyy-MM-dd\r\nhh:mm:ss");
        ui->accountTransactionsTableWidget->setItem(i,0,new QTableWidgetItem(currentDateTime));

        // 区块高度
        if( transactionInfo.blockNum > 0)
        {
            ui->accountTransactionsTableWidget->setItem(i,1,new QTableWidgetItem( QString::number(transactionInfo.blockNum) ));
        }
        else
        {
            ui->accountTransactionsTableWidget->setItem(i,1,new QTableWidgetItem( tr("unconfirmed") ));
            ui->accountTransactionsTableWidget->item(i,1)->setTextColor(QColor(200,200,200));
        }

        // 交易ID
        ui->accountTransactionsTableWidget->setItem(i,2,new QTableWidgetItem( transactionInfo.trxId));


        TransactionDetail detail = getDetail(transactionInfo);
        // 对方账户
        ui->accountTransactionsTableWidget->setItem(i,3,new QTableWidgetItem( CDC::getInstance()->addressToName(detail.opposite)));

        // 金额
        AssetInfo assetInfo = CDC::getInstance()->assetInfoMap.value(detail.assetAmount.assetId);
        unsigned long long amount = 0;
        if( detail.bothMyAccount || transactionInfo.isConfirmed == false || detail.assetAmount.amount == 0)
        {
            amount = detail.assetAmount.amount;
        }
        else
        {
            amount = detail.assetAmount.amount - transactionInfo.fee;
        }

        if( detail.type == 1)
        {
            ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( "-" + getBigNumberString(amount,assetInfo.precision) + " "
                                                                                  + assetInfo.symbol));
            ui->accountTransactionsTableWidget->item(i,4)->setTextColor(QColor(239,104,60));
        }
        else if( detail.type == 2 || detail.type == 3 || detail.type == 4 )
        {
            ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( "+" + getBigNumberString(amount,assetInfo.precision) + " "
                                                                                  + assetInfo.symbol));
            ui->accountTransactionsTableWidget->item(i,4)->setTextColor(QColor(113,203,90));
        }
        else
        {
            ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem( getBigNumberString(amount,assetInfo.precision) + " "
                                                                                  + assetInfo.symbol));
        }

        if( transactionInfo.isConfirmed == false)
        {
            ui->accountTransactionsTableWidget->item(i,4)->setTextColor(QColor(200,200,200));
        }

        // 手续费
        ui->accountTransactionsTableWidget->setItem(i, 5, new QTableWidgetItem(getBigNumberString(transactionInfo.fee, assetInfo.precision)));

        // 备注
        ui->accountTransactionsTableWidget->setItem(i,6,new QTableWidgetItem( detail.memo));

        for(int j = 0; j < 7; j++)
        {
            ui->accountTransactionsTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
        }

    }

}

AccountPage::TransactionDetail AccountPage::getDetail(TransactionInfo info)
{
    TransactionDetail result;    
    if( info.trxId.isEmpty())   return result;
    if( !info.isMarket)
    {
        Entry entry;
        if( info.entries.size() > 1)
        {
            entry = info.entries.at(1);
            result.bothMyAccount = true;
        }
        else
        {
            entry = info.entries.at(0);
            result.bothMyAccount = false;
        }

        if(entry.toAccount.isEmpty())
        {
//            result.opposite = QString::fromLocal8Bit("调用合约");
            result.opposite = tr("call contract");
            result.type     = 7;
            result.assetAmount = entry.amount;
            result.memo     = entry.memo;
        }
        else if( entry.fromAccount == accountName || entry.fromAccount == CDC::getInstance()->addressMap.value(accountName).ownerAddress)
        {
            if( entry.toAccount == accountName)
            {
                result.opposite = entry.toAccount;
                result.type     = 0;
                result.assetAmount = entry.amount;
                result.memo     = entry.memo;
            }
            else
            {
                result.opposite = entry.toAccount;
                result.type     = 1;
                result.assetAmount = entry.amount;
                result.memo     = entry.memo;
            }
        }
        else
        {
            if( entry.toAccount == accountName || entry.toAccount == CDC::getInstance()->addressMap.value(accountName).ownerAddress)
            {

                result.assetAmount = entry.amount;
                result.memo = entry.memo;

                if(entry.memo.startsWith("pay pow reward"))
                {
                    result.type     = 3;
                    result.opposite = tr("POW mining reward");
                }
                else if(entry.memo.startsWith("pay pos reward"))
                {
                    result.type     = 4;
                    result.opposite = tr("POS mining reward");
                }
                else
                {
                    result.type     = 2;
                    result.opposite = entry.fromAccount;
                }
            }
        }

    }

    return result;
}

void AccountPage::showContractTransactions()
{
    ui->accountTransactionsTableWidget->setColumnHidden(6,true);

    QStringList contracts = CDC::getInstance()->ERC20TokenInfoMap.keys();
    QString contractAddress = contracts.at(ui->assetComboBox->currentIndex() - 1);
    QString accountAddress = CDC::getInstance()->addressMap.value(accountName).ownerAddress;

    ERC20TokenInfo info = CDC::getInstance()->ERC20TokenInfoMap.value(contractAddress);

    ContractTransactionVector vector = CDC::getInstance()->accountContractTransactionMap.value(accountAddress + "-" + contractAddress);

    ui->accountTransactionsTableWidget->setRowCount(0);
    if( vector.size() < 1)
    {
        ui->prePageBtn->hide();
        ui->numberLabel->hide();
        ui->pageLineEdit->hide();
        ui->pageLabel->hide();
        ui->nextPageBtn->hide();
        ui->initLabel->show();
        return;
    }
    ui->prePageBtn->show();
    ui->numberLabel->show();
    ui->pageLineEdit->show();
    ui->pageLabel->show();
    ui->nextPageBtn->show();
    ui->initLabel->hide();

    int size = vector.size();
    ui->numberLabel->setText( tr("total ") + QString::number( size) + tr(" ,"));
    ui->pageLabel->setText( "/" + QString::number( (size - 1)/10 + 1 ) );

    int rowCount = size - (currentPageIndex - 1) * 10;
    if( rowCount > 10 )  rowCount = 10;  // 一页最多显示10行
    ui->accountTransactionsTableWidget->setRowCount(rowCount);

    for(int i = rowCount - 1; i > -1; i--)
    {
        ui->accountTransactionsTableWidget->setRowHeight(i,57);
        ContractTransaction transaction = vector.at(  size - ( i + 1) - (currentPageIndex - 1) * 10 );

        // 时间戳
        QString date = transaction.timeStamp;
        date.replace(QString("T"),QString(" "));
        QDateTime time = QDateTime::fromString(date, "yyyy-MM-dd hh:mm:ss");
        time = time.addSecs(28800);       // 时间加8小时
        QString currentDateTime = time.toString("yyyy-MM-dd\r\nhh:mm:ss");
        ui->accountTransactionsTableWidget->setItem(i,0,new QTableWidgetItem(currentDateTime));

        // 区块高度
        ui->accountTransactionsTableWidget->setItem(i,1,new QTableWidgetItem( QString::number(transaction.blockNum) ));

        // 交易ID
        ui->accountTransactionsTableWidget->setItem(i,2,new QTableWidgetItem( transaction.trxId));

        // 对方账户
        QString opposite;
        int type = 0;   // 发送 0    接收 1  自己转自己 2
        if( transaction.fromAddress == accountAddress)
        {
            if( transaction.toAddress == accountAddress)
            {
                type = 2;
            }
            else
            {
                type = 0;
            }
            opposite = transaction.toAddress;
        }
        else
        {
            opposite = transaction.fromAddress;
            type = 1;
        }

        ui->accountTransactionsTableWidget->setItem(i,3,new QTableWidgetItem(CDC::getInstance()->addressToName(opposite)));

        // 金额
        if( type == 0)
        {
            ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem("-" + getBigNumberString(transaction.amount, info.precision) + " "
                                                                                  + info.symbol));
            ui->accountTransactionsTableWidget->item(i,4)->setTextColor(QColor(239,104,60));
        }
        else if( type == 1)
        {
            ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem("+" + getBigNumberString(transaction.amount, info.precision) + " "
                                                                                  + info.symbol));
            ui->accountTransactionsTableWidget->item(i,4)->setTextColor(QColor(113,203,90));
        }
        else
        {
            ui->accountTransactionsTableWidget->setItem(i,4,new QTableWidgetItem(getBigNumberString(transaction.amount, info.precision) + " "
                                                                                  + info.symbol));
        }


        // 手续费
        if( type == 1)
        {
            ui->accountTransactionsTableWidget->setItem(i,5,new QTableWidgetItem(QString::number(0)));
        }
        else
        {
            ui->accountTransactionsTableWidget->setItem(i,5,new QTableWidgetItem(getBigNumberString(transaction.fee, 100000000)));
        }


        for(int j = 0; j < 6; j++)
        {
            ui->accountTransactionsTableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void AccountPage::on_prePageBtn_clicked()
{
    ui->accountTransactionsTableWidget->scrollToTop();
    if( currentPageIndex == 1) return;
    currentPageIndex--;
    showTransactions();
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

}

void AccountPage::on_nextPageBtn_clicked()
{
    int totalPageNum = ui->pageLabel->text().remove("/").toInt();
    if(  currentPageIndex >= totalPageNum )  return;

    currentPageIndex++;
    showTransactions();
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    ui->accountTransactionsTableWidget->scrollToTop();
}

void AccountPage::on_pageLineEdit_editingFinished()
{
    currentPageIndex = ui->pageLineEdit->text().toInt();
    showTransactions();
}

void AccountPage::on_pageLineEdit_textEdited(const QString &arg1)
{
    if( arg1.at(0) == '0')
    {
        ui->pageLineEdit->setText( arg1.mid(1));
        return;
    }
    int totalPageNum = ui->pageLabel->text().remove("/").toInt();

    if( arg1.toInt() > totalPageNum)
    {
        ui->pageLineEdit->setText( QString::number( totalPageNum));
        return;
    }
}

void AccountPage::on_assetComboBox_currentIndexChanged(int index)
{
    if( assetUpdating)  return;

    currentPageIndex = 1;
    ui->pageLineEdit->setText( QString::number(currentPageIndex));

    if( index == 0)
    {
        CDC::getInstance()->currentTokenAddress = "";
    }
    else
    {
        CDC::getInstance()->currentTokenAddress = CDC::getInstance()->ERC20TokenInfoMap.keys().at(index - 1);
    }

    init();
    updateTransactionsList();
    updateBalance();
    showTransactions();
}

void AccountPage::on_accountTransactionsTableWidget_cellPressed(int row, int column)
{
    if( column == 2 || column == 3 || column == 6 )
    {
        ShowContentDialog showContentDialog( ui->accountTransactionsTableWidget->item(row, column)->text(),this);

        int x = ui->accountTransactionsTableWidget->columnViewportPosition(column) + ui->accountTransactionsTableWidget->columnWidth(column) / 2
                - showContentDialog.width() / 2;
        int y = ui->accountTransactionsTableWidget->rowViewportPosition(row) - 10 + ui->accountTransactionsTableWidget->horizontalHeader()->height();

        showContentDialog.move( ui->accountTransactionsTableWidget->mapToGlobal( QPoint(x, y)));
        showContentDialog.exec();

        return;
    }
}

void AccountPage::on_qrcodeBtn_clicked()
{
    QRCodeDialog qrcodeDialog(CDC::getInstance()->addressMapValue(accountName).ownerAddress);
    qrcodeDialog.move(ui->qrcodeBtn->mapToGlobal( QPoint(20,0)));
    qrcodeDialog.exec();
}
