#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QScrollBar>
#include <QMovie>
#include <math.h>

#include "mainpage.h"
#include "ui_mainpage.h"
#include "blockchain.h"
#include "debug_log.h"
#include "namedialog.h"
#include "deleteaccountdialog.h"
#include "rpcthread.h"
#include "exportdialog.h"
#include "importdialog.h"
#include "commondialog.h"
#include "showcontentdialog.h"
#include "incomecellwidget.h"
#include "control/rightclickmenudialog.h"
#include "control/chooseaddaccountdialog.h"
#include "dialog/renamedialog.h"

MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    hasDelegateOrNot(false),
    refreshOrNot(true),
    currentAccountIndex(-1),
    assetUpdating(false),
    ui(new Ui::MainPage)
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;

    ui->setupUi(this);

    connect( CDC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(244,244,242));
    setPalette(palette);

    ui->accountTableWidget->installEventFilter(this);
    ui->accountTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->accountTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->accountTableWidget->setFocusPolicy(Qt::NoFocus);
//    ui->accountTableWidget->setFrameShape(QFrame::NoFrame);
    ui->accountTableWidget->setMouseTracking(true);
    previousColorRow = 0;
    ui->accountTableWidget->horizontalHeader()->setSectionsClickable(false);
    ui->accountTableWidget->horizontalHeader()->setFixedHeight(47);
    ui->accountTableWidget->horizontalHeader()->setVisible(true);
    ui->accountTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);



    ui->accountTableWidget->setColumnWidth(0,173);
    ui->accountTableWidget->setColumnWidth(1,424);
    ui->accountTableWidget->setColumnWidth(2,224);

    QString language = CDC::getInstance()->language;
    if( language.isEmpty())
    {
        retranslator("Simplified Chinese");
    }
    else
    {
        retranslator(language);
    }

#ifdef WIN32
    ui->assetComboBox->setStyleSheet("QComboBox{border: 1px solid gray;border-radius: 3px;padding: 1px 2px 1px 8px;}"
                  "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 20px;"
                  "border-left-width: 1px;border-left-color: darkgray;border-left-style: solid;"
                  "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                  "QComboBox::down-arrow {image: url(:/pic/pic2/comboBoxArrow.png);}"
                  );
#endif

    // 由于首页是第一个页面，第一次打开先等待x秒钟 再 updateAccountList
    QTimer::singleShot(500, this, SLOT(init()));

    ui->accountTableWidget->hide();

    ui->loadingWidget->setGeometry(0,93,827,448);
    ui->loadingLabel->move(314,101);

    ui->initLabel->hide();

    ui->scanBtn->setStyleSheet("QToolButton{background-image:url(:/pic/cplpic/refresh2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    gif = new QMovie(":/pic/cplpic/refresh2.gif");
    gif->setScaledSize( QSize(16,16));
    ui->scanLabel->setMovie(gif);
    ui->scanLabel->hide();
    connect(gif,SIGNAL(finished()),ui->scanLabel,SLOT(hide()));
    connect(gif,SIGNAL(finished()),ui->scanBtn,SLOT(show()));

    ui->scanBtn->setToolTip(tr("If the balance or transaction record is incorrect, rescan the blockchain"));


    DLOG_QT_WALLET_FUNCTION_END;
}

MainPage::~MainPage()
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;

    delete ui;

	DLOG_QT_WALLET_FUNCTION_END;
}

QString toThousandFigure( int);

void MainPage::importAccount()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    ImportDialog importDialog;
    connect(&importDialog,SIGNAL(accountImported()),this,SLOT(refresh()));
    importDialog.pop();

    emit refreshAccountInfo();

    DLOG_QT_WALLET_FUNCTION_END;
}

void MainPage::addAccount()
{
    NameDialog nameDialog;
    QString name = nameDialog.pop();

    if( !name.isEmpty())
    {

        emit showShadowWidget();
        CDC::getInstance()->write("wallet_account_create " + name + '\n');
        QString result = CDC::getInstance()->read();
        emit hideShadowWidget();

        if(result.left(5) == "20017")
        {
            qDebug() << "wallet_account_create " + name + '\n'  << result;
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Failed"));
            commonDialog.pop();
            return;
        }
        else
        {
            if( result.mid(0,1) == "H")
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("Please backup the private key of this account!!!") );
                commonDialog.pop();

                ExportDialog exportDialog(name);
                exportDialog.pop();
            }

        }
        emit newAccount(name);
    }
}


void MainPage::updateAccountList()
{
    mutexForConfigFile.lock();
    CDC::getInstance()->configFile->beginGroup("/accountInfo");
    QStringList keys = CDC::getInstance()->configFile->childKeys();

    int size = keys.size();
    if( size == 0)  // 如果还没有账户
    {
        ui->initLabel->show();
        ui->accountTableWidget->hide();
        ui->loadingWidget->hide();
        CDC::getInstance()->configFile->endGroup();
        mutexForConfigFile.unlock();
        return;
    }
    else
    {
        ui->initLabel->hide();
        ui->accountTableWidget->show();
        ui->loadingWidget->show();
    }

    int assetIndex = ui->assetComboBox->currentIndex();
    if( assetIndex <= 0)
    {
        assetIndex = 0;

        AssetInfo info = CDC::getInstance()->assetInfoMap.value(assetIndex);
        QTableWidgetItem* item = ui->accountTableWidget->horizontalHeaderItem(2);
        item->setText(tr("Balance/") + info.symbol);


        ui->accountTableWidget->setRowCount(size);
        for( int i = size - 1; i > -1; i--)
        {
            QString accountName = CDC::getInstance()->configFile->value( keys.at( i)).toString();
            int rowNum = i;

            ui->accountTableWidget->setRowHeight(rowNum,57);
            ui->accountTableWidget->setItem(rowNum,0,new QTableWidgetItem(accountName));
            ui->accountTableWidget->setItem(rowNum,1,new QTableWidgetItem(CDC::getInstance()->addressMapValue(accountName).ownerAddress));


            AssetBalanceMap map = CDC::getInstance()->accountBalanceMap.value(accountName);
            ui->accountTableWidget->setItem(rowNum,2,new QTableWidgetItem(getBigNumberString(map.value(assetIndex),info.precision)));

    //        ui->accountTableWidget->setItem(rowNum,2,new QTableWidgetItem(Hcash::getInstance()->balanceMapValue(accountName).remove(ASSET_NAME)));
            ui->accountTableWidget->item(rowNum,0)->setTextAlignment(Qt::AlignCenter);
            ui->accountTableWidget->item(rowNum,1)->setTextAlignment(Qt::AlignCenter);
            ui->accountTableWidget->item(rowNum,2)->setTextAlignment(Qt::AlignCenter);
            ui->accountTableWidget->item(rowNum,2)->setTextColor(Qt::red);

            if( rowNum == currentAccountIndex)
            {
                for( int i = 0; i < 3; i++)
                {
                    ui->accountTableWidget->item(currentAccountIndex,i)->setBackgroundColor(QColor(245,248,248,150));
                }
            }
        }

    }
    else
    {
        // 如果是合约资产
        QStringList contracts = CDC::getInstance()->ERC20TokenInfoMap.keys();
        QString contractAddress = contracts.at(assetIndex - 1);

        ERC20TokenInfo info = CDC::getInstance()->ERC20TokenInfoMap.value(contractAddress);
        QTableWidgetItem* item = ui->accountTableWidget->horizontalHeaderItem(2);
        item->setText(tr("Balance/") + info.symbol);

        ui->accountTableWidget->setRowCount(size);
        for( int i = size - 1; i > -1; i--)
        {
            QString accountName = CDC::getInstance()->configFile->value( keys.at( i)).toString();
            int rowNum = i;

            ui->accountTableWidget->setRowHeight(rowNum,57);
            ui->accountTableWidget->setItem(rowNum,0,new QTableWidgetItem(accountName));
            ui->accountTableWidget->setItem(rowNum,1,new QTableWidgetItem(CDC::getInstance()->addressMapValue(accountName).ownerAddress));


            ContractBalanceMap map = CDC::getInstance()->accountContractBalanceMap.value(CDC::getInstance()->addressMap.value(accountName).ownerAddress);
            ui->accountTableWidget->setItem(rowNum,2,new QTableWidgetItem(getBigNumberString(map.value(contractAddress),info.precision)));
    //        ui->accountTableWidget->setItem(rowNum,2,new QTableWidgetItem(Hcash::getInstance()->balanceMapValue(accountName).remove(ASSET_NAME)));
            ui->accountTableWidget->item(rowNum,0)->setTextAlignment(Qt::AlignCenter);
            ui->accountTableWidget->item(rowNum,1)->setTextAlignment(Qt::AlignCenter);
            ui->accountTableWidget->item(rowNum,2)->setTextAlignment(Qt::AlignCenter);
            ui->accountTableWidget->item(rowNum,2)->setTextColor(Qt::red);

            if( rowNum == currentAccountIndex)
            {
                for( int i = 0; i < 3; i++)
                {
                    ui->accountTableWidget->item(currentAccountIndex,i)->setBackgroundColor(QColor(245,248,248,150));
                }
            }
        }

    }

    CDC::getInstance()->configFile->endGroup();
    mutexForConfigFile.unlock();

    ui->loadingWidget->hide();

    
}

void MainPage::on_addAccountBtn_clicked()
{
    ChooseAddAccountDialog* chooseAddAccountDialog = new ChooseAddAccountDialog(this);
    chooseAddAccountDialog->move( ui->addAccountBtn->mapToGlobal( QPoint(10,-79) ) );
    connect( chooseAddAccountDialog, SIGNAL(newAccount()), this, SLOT( addAccount()));
    connect( chooseAddAccountDialog, SIGNAL(importAccount()), this, SLOT( importAccount()));
    chooseAddAccountDialog->exec();
}

void MainPage::on_accountTableWidget_cellClicked(int row, int column)
{
    

//    AccountCellWidget* cellWidget = static_cast<AccountCellWidget*>( ui->accountTableWidget->cellWidget(row,0) );

//    emit openAccountPage( cellWidget->accountName);

    emit openAccountPage( ui->accountTableWidget->item(row,0)->text());

    //    showDetailWidget( cellWidget->accountName );

//        showDetailWidget( ui->accountTableWidget->item(row,0)->text() );

//    if( currentAccountIndex >= 0 && currentAccountIndex != row)
//    {
//        AccountCellWidget* cellWidget2 = static_cast<AccountCellWidget*>( ui->accountTableWidget->cellWidget(currentAccountIndex,0) );
//        cellWidget2->setBackgroundColor( 255,255,255);
//        for( int i = 1; i < 3; i++)
//        {
//            ui->accountTableWidget->item(currentAccountIndex,i)->setBackgroundColor(QColor(255,255,255));
//        }
//    }

//    cellWidget->setBackgroundColor( 245,248,248,150);
//    for( int i = 1; i < 3; i++)
//    {
//        ui->accountTableWidget->item(row,i)->setBackgroundColor(QColor(245,248,248,150));
//    }

//    currentAccountIndex = row;

	
}


void MainPage::on_accountTableWidget_cellEntered(int row, int column)
{

    if( ui->accountTableWidget->rowCount() > 0)
        {
            for( int i = 0; i < 3; i++)
            {
                if( ui->accountTableWidget->item(previousColorRow,i) != NULL)
                {
                    ui->accountTableWidget->item(previousColorRow,i)->setBackgroundColor(QColor(255,255,255));
                }
            }
        }

        for( int i = 0; i < 3; i++)
        {
            if( ui->accountTableWidget->item(row,i) != NULL)
            {
                ui->accountTableWidget->item(row,i)->setBackgroundColor(QColor(245,248,248));
            }
        }

        previousColorRow = row;


}

int tableWidgetPosToRow(QPoint pos, QTableWidget* table);

void MainPage::refresh()
{
    qDebug() << "mainpage refresh"   << refreshOrNot;
    if( !refreshOrNot) return;

    updateAccountList();
    updateTotalBalance();

}

void MainPage::init()
{
    getAssets();
    if( !CDC::getInstance()->currentTokenAddress.isEmpty())
    {
        ui->assetComboBox->setCurrentText(CDC::getInstance()->ERC20TokenInfoMap.value(CDC::getInstance()->currentTokenAddress).symbol);
    }

    updateAccountList();
    updateTotalBalance();

//    showWarningDialog();
}

void MainPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(228,228,228),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(220,195,145),Qt::SolidPattern));
    painter.drawRect(-1,-1,858,68);

}

void MainPage::retranslator(QString language)
{
    ui->retranslateUi(this);

    if( language == "Simplified Chinese")
    {

    }
    else if( language == "English")
    {

    }
}

void MainPage::jsonDataUpdated(QString id)
{
    if( id.mid(0,37) == "id_wallet_delegate_pay_balance_query_")
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        int pos = result.indexOf("\"pay_balance\":") + 14;
        QString payBal = result.mid( pos, result.indexOf("}", pos) - pos );
        payBal.remove("\"");
        CDC::getInstance()->delegateSalaryMap.insert(id.mid(37), payBal.toInt() / 10000.0);
        return;
    }


    if( id == "id_wallet_delegate_withdraw_pay")
    {
        QString result = CDC::getInstance()->jsonDataValue(id);
        if( result.mid(0,9) == "\"result\":")
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Withdraw succeeded!") );
            commonDialog.pop();

            refresh();
        }
        else if( result.mid(0,8) == "\"error\":")
        {
            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( tr("Withdraw failed!") );
            commonDialog.pop();
        }

        return;
    }

}

void MainPage::updateTotalBalance()
{
    unsigned long long totalBalance = 0;
    int assetIndex = ui->assetComboBox->currentIndex();
    if( assetIndex <= 0)
    {
        assetIndex = 0;
        AssetInfo info = CDC::getInstance()->assetInfoMap.value(assetIndex);
        foreach (QString key, CDC::getInstance()->accountBalanceMap.keys())
        {
            AssetBalanceMap map = CDC::getInstance()->accountBalanceMap.value(key);
            totalBalance += map.value(assetIndex);
        }
        qDebug() << totalBalance << ", " << info.precision;
        ui->totalBalanceLabel->setText("<body><font style=\"font-size:18px\" color=#ff0000>" +
                                       getBigNumberString(totalBalance,info.precision) +
                                       "</font><font style=\"font-size:12px\" color=#000000> " +
                                       info.symbol +"</font></body>" );
        ui->totalBalanceLabel->adjustSize();
        ui->scanBtn->move(ui->totalBalanceLabel->x() + ui->totalBalanceLabel->width() + 10,ui->totalBalanceLabel->y() + 7);
        ui->scanLabel->move(ui->totalBalanceLabel->x() + ui->totalBalanceLabel->width() + 9,ui->totalBalanceLabel->y() + 5);

    }
    else
    {
        // 如果是合约资产
        QStringList contracts = CDC::getInstance()->ERC20TokenInfoMap.keys();
        QString contractAddress = contracts.at(assetIndex - 1);

        ERC20TokenInfo info = CDC::getInstance()->ERC20TokenInfoMap.value(contractAddress);

        QStringList keys = CDC::getInstance()->addressMap.keys();
        foreach (QString key, keys)
        {
            QString accountAddress = CDC::getInstance()->addressMap.value(key).ownerAddress;
            totalBalance += CDC::getInstance()->accountContractBalanceMap.value(accountAddress).value(contractAddress);
        }
        ui->totalBalanceLabel->setText( "<body><font style=\"font-size:18px\" color=#ff0000>" + getBigNumberString(totalBalance,info.precision) + "</font><font style=\"font-size:12px\" color=#000000> " + info.symbol +"</font></body>" );
        ui->totalBalanceLabel->adjustSize();
        ui->scanBtn->move(ui->totalBalanceLabel->x() + ui->totalBalanceLabel->width() + 10,ui->totalBalanceLabel->y() + 7);
        ui->scanLabel->move(ui->totalBalanceLabel->x() + ui->totalBalanceLabel->width() + 9,ui->totalBalanceLabel->y() + 5);
    }
}

void MainPage::updatePending()
{
    
    mutexForPending.lock();

    if( !CDC::getInstance()->pendingFile->open(QIODevice::ReadWrite))
    {
        qDebug() << "pending.dat not exist";
        return;
    }

    QByteArray ba = QByteArray::fromBase64( CDC::getInstance()->pendingFile->readAll());
    QString str(ba);
    QStringList strList = str.split(";");
    strList.removeLast();

    mutexForAddressMap.lock();
    QStringList keys = CDC::getInstance()->addressMap.keys();
    mutexForAddressMap.unlock();

    mutexForConfigFile.lock();
    CDC::getInstance()->configFile->beginGroup("recordId");
    QStringList recordKeys = CDC::getInstance()->configFile->childKeys();
    CDC::getInstance()->configFile->endGroup();


    foreach (QString ss, strList)
    {
        QStringList sList = ss.split(",");

        if( !keys.contains( sList.at(1)) && keys.size() > 0)   // 如果账号被删除了， 删掉pending 中的记录   keys.size() 防止刚启动 addressmap为空
        {
            strList.removeAll( ss);
            continue;
        }
        // 如果config中recordId会被置为1， 则删除该记录
        if( CDC::getInstance()->configFile->value("recordId/" + sList.at(0)).toInt() != 0 )
        {
            strList.removeAll( ss);
            continue;
        }

        // 如果config中recordId被删除了， 则删除该记录
        if( !CDC::getInstance()->configFile->contains("recordId/" + sList.at(0)))
        {
            strList.removeAll( ss);
            continue;
        }

    }
    mutexForConfigFile.unlock();


    ba.clear();
    foreach (QString ss, strList)
    {
        ba += QString( ss + ";").toUtf8();
    }
    ba = ba.toBase64();
    CDC::getInstance()->pendingFile->resize(0);
    QTextStream ts(CDC::getInstance()->pendingFile);
    ts << ba;
    CDC::getInstance()->pendingFile->close();

    mutexForPending.unlock();
    
}

//  tablewidget 从 pos 获取 item（每行第0个）
int tableWidgetPosToRow(QPoint pos, QTableWidget* table)
{
    int headerHeight = 47;
    int rowHeight = 57;

    // 获得当前滚动条的位置
    int scrollBarValue = table->verticalScrollBar()->sliderPosition();

    if( pos.y() < headerHeight || pos.y() > table->height())
    {
        return -1;
    }
    else
    {
#ifdef WIN32	
        int rowCount = floor( (pos.y() - headerHeight) / rowHeight) + scrollBarValue;
#else
        int rowCount = floor((pos.y() - headerHeight) / rowHeight ) + scrollBarValue;
#endif
        return rowCount;
    }

}

bool MainPage::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->accountTableWidget)
    {

        if( e->type() == QEvent::ContextMenu)
        {
            QContextMenuEvent* contextMenuEvent = static_cast<QContextMenuEvent*>(e);
            QPoint pos = contextMenuEvent->pos();
            pos -= QPoint(0,47);
            int row = ui->accountTableWidget->row(ui->accountTableWidget->itemAt(pos));
            if( row == -1)    return false;

            if( ui->accountTableWidget->item(row,0) == NULL)   return false;
            QString name = ui->accountTableWidget->item(row,0)->text();
            RightClickMenuDialog* rightClickMenuDialog = new RightClickMenuDialog( name, this);
            rightClickMenuDialog->move( ui->accountTableWidget->mapToGlobal(contextMenuEvent->pos()) );
            connect( rightClickMenuDialog, SIGNAL(transferFromAccount(QString)), this, SIGNAL(showTransferPage(QString)));
            connect( rightClickMenuDialog, SIGNAL(renameAccount(QString)), this, SLOT(renameAccount(QString)));
            connect( rightClickMenuDialog, SIGNAL(exportAccount(QString)), this, SLOT(showExportDialog(QString)));
            connect( rightClickMenuDialog, SIGNAL(deleteAccount(QString)), this, SLOT(deleteAccount(QString)));
            rightClickMenuDialog->exec();

            return true;


        }
    }

    return QWidget::eventFilter(watched,e);
}

void MainPage::showWarningDialog()
{
    if( CDC::getInstance()->firstUse)
    {
        CDC::getInstance()->firstUse = false;
        CDC::getInstance()->configFile->setValue("settings/firstUse",false);
    }
}


void MainPage::showExportDialog(QString name)
{

    ExportDialog exportDialog(name);
    exportDialog.pop();
}

void MainPage::withdrawSalary(QString name, QString salary)
{
//    double amount = salary.toDouble() - 0.01;
//    if( amount > 0.000001)
//    {
//        CommonDialog commonDialog(CommonDialog::OkAndCancel);
//        commonDialog.setText( tr("Sure to withdraw your salary?"));
//        if( commonDialog.pop())
//        {
//            Hcash::getInstance()->postRPC( toJsonFormat( "id_wallet_delegate_withdraw_pay", "wallet_delegate_withdraw_pay",
//                                                          QStringList() << name <<  name << QString::number(amount) ));
//        }
//    }

}

void MainPage::renameAccount(QString name)
{
    RenameDialog renameDialog;
    QString newName = renameDialog.pop();

    if( !newName.isEmpty() && newName != name)
    {
        CDC::getInstance()->write("wallet_account_rename " + name + " " + newName + '\n');
        QString result = CDC::getInstance()->read();
        qDebug() << result;
        if( result.mid(0,2) == "OK")
        {
            mutexForConfigFile.lock();
            CDC::getInstance()->configFile->beginGroup("/accountInfo");
            QStringList keys = CDC::getInstance()->configFile->childKeys();
            foreach (QString key, keys)
            {
                if( CDC::getInstance()->configFile->value(key) == name)
                {
                    CDC::getInstance()->configFile->setValue(key, newName);
                    break;
                }
            }
            CDC::getInstance()->configFile->endGroup();
            mutexForConfigFile.unlock();
            CDC::getInstance()->balanceMapInsert( newName, CDC::getInstance()->balanceMapValue(name));
            CDC::getInstance()->balanceMapRemove(name);
            CDC::getInstance()->registerMapInsert( newName, CDC::getInstance()->registerMapValue(name));
            CDC::getInstance()->registerMapRemove(name);
            CDC::getInstance()->addressMapInsert( newName, CDC::getInstance()->addressMapValue(name));
            CDC::getInstance()->addressMapRemove(name);

//            detailWidget->accountName = newName;
            emit newAccount(newName);

        }
        else
        {
            return;
        }


    }
}

void MainPage::deleteAccount(QString name)
{
    DeleteAccountDialog deleteACcountDialog( name);
    deleteACcountDialog.pop();
//    if( deleteACcountDialog.pop())
//    {
//        previousColorRow = 0;
//        currentAccountIndex = -1;
//        refresh();
//    }
}


//void MainPage::hideDetailWidget()
//{
//    detailOrNot = false;

//    ui->accountTableWidget->setColumnWidth(0,173);
//    ui->accountTableWidget->setColumnWidth(1,424);
//    ui->accountTableWidget->setColumnWidth(2,154);

//    ui->addAccountBtn->move(675,422);

//    detailWidget->dynamicHide();
//}

void MainPage::on_scanBtn_clicked()
{
    ui->scanLabel->show();
    ui->scanLabel->raise();
    gif->start();
    ui->scanBtn->hide();

    CDC::getInstance()->postRPC( toJsonFormat( "id_scan", "scan", QStringList() << "0"));
}


void MainPage::getAssets()
{
    assetUpdating = true;
    ui->assetComboBox->clear();
    ui->assetComboBox->addItem(ASSET_NAME);
    qDebug() << CDC::getInstance()->ERC20TokenInfoMap.size();
    foreach (QString key, CDC::getInstance()->ERC20TokenInfoMap.keys())
    {
        qDebug() << key;
        ui->assetComboBox->addItem( CDC::getInstance()->ERC20TokenInfoMap.value(key).symbol);
    }
    assetUpdating = false;
}

void MainPage::on_assetComboBox_currentIndexChanged(int index)
{
    if( assetUpdating)  return;

    if( index == 0)
    {
        CDC::getInstance()->currentTokenAddress = "";
    }
    else
    {
        CDC::getInstance()->currentTokenAddress = CDC::getInstance()->ERC20TokenInfoMap.keys().at(index - 1);
    }

    updateAccountList();
    updateTotalBalance();
}
