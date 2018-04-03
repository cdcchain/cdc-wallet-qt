// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <QPainter>
#include <QLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>
#include <QMenu>
#include <QApplication>

#include "firstlogin.h"
#include "normallogin.h"
#include "mainpage.h"
#include "accountpage.h"
#include "transferpage.h"
#include "bottombar.h"
#include "lockpage.h"
#include "titlebar.h"
#include "frame.h"
#include "blockchain.h"
#include "debug_log.h"
#include "waitingforsync.h"
#include "functionbar.h"
#include "contactpage.h"
#include "selectwalletpathwidget.h"
#include "rpcthread.h"
#include "control/shadowwidget.h"
#include "control/showbottombarwidget.h"
#include "assetpage.h"
#include "commondialog.h"
#include "consolewidget.h"
#include "multisigpage.h"

Frame::Frame(): timer(NULL),
    firstLogin(NULL),
    normalLogin(NULL),
    mainPage(NULL),
    accountPage(NULL),
    transferPage(NULL),
    bottomBar(NULL),
    centralWidget(NULL),
    lockPage(NULL),
    titleBar(NULL),
    contactPage(NULL),
    functionBar(NULL),
    shadowWidget(NULL),
    timerForAutoRefresh(NULL),
    waitingForSync(NULL),
    selectWalletPathWidget(NULL),
    assetPage(NULL),
    multiSigPage(NULL),
    needToRefresh(false)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

#ifdef TARGET_OS_MAC
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint |Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);    // mac下设为window popup属性的对话框会有问题
#else
    setWindowFlags(Qt::Window|Qt::FramelessWindowHint |Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);
#endif

#ifdef WIN32
    ::SystemParametersInfo(SPI_GETWORKAREA , 0 , &this->rtConfined , 0);
    ::GetWindowRect(::GetDesktopWindow() , &this->rtDefault);
#endif

    setFrameShape(QFrame::NoFrame);
//    setMouseTracking(true);

//    setStyleSheet("Frame{background-color:white; border: 4px solid #CCCCCC;border-radius:5px;}"
//                  "QScrollBar:vertical{width:8px;background:transparent;margin:0px,0px,0px,0px;padding-top:2px;padding-bottom:3px;}"
//                  "QScrollBar::handle:vertical{width:8px;background:rgba(130,137,143,20%);border-radius:4px;min-height:20;}"
//                  "QScrollBar::handle:vertical:hover{width:8px;background:rgba(130,137,143,100%);border-radius:4px;min-height:20;}"
//                  "QScrollBar::add-line:vertical{height:9px;width:8px;border-image:url(:/images/a/3.png);subcontrol-position:bottom;}"
//                  "QScrollBar::sub-line:vertical{height:9px;width:8px;border-image:url(:/images/a/1.png);subcontrol-position:top;}"
//                  "QScrollBar::add-line:vertical:hover{height:9px;width:8px;border-image:url(:/images/a/4.png);subcontrol-position:bottom;}"
//                  "QScrollBar::sub-line:vertical:hover{height:9px;width:8px;border-image:url(:/images/a/2.png);subcontrol-position:top;}"
//                  "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background:rgba(0,0,0,0%);border-radius:4px;}"
//                  );

    mouse_press = false;
    currentPageNum = 0;
    lastPage = "";
    currentAccount = "";
    quitTimerCount = 100;

    connect(CDC::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    QString language = CDC::getInstance()->language;
    if( language.isEmpty())
    {
        setLanguage("Simplified Chinese");
    }
    else
    {
        setLanguage(language);
    }

    setGeometry(240,110,960,580);
    shadowWidget = new ShadowWidget(this);
    shadowWidget->init(this->size());

    //  如果是第一次使用(未设置 blockchain 路径)
    mutexForConfigFile.lock();

    if( !CDC::getInstance()->configFile->contains("/settings/chainPath") )
    {
        selectWalletPathWidget = new SelectWalletPathWidget(this);
        selectWalletPathWidget->setAttribute(Qt::WA_DeleteOnClose);
        selectWalletPathWidget->move(0,0);
        connect( selectWalletPathWidget,SIGNAL(enter()),this,SLOT(showWaittingForSyncWidget()));
        connect( selectWalletPathWidget,SIGNAL(minimum()),this,SLOT(showMinimized()));
        connect( selectWalletPathWidget,SIGNAL(closeWallet()),qApp,SLOT(quit()));
        connect( selectWalletPathWidget,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
        connect( selectWalletPathWidget,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));

        selectWalletPathWidget->show();

    }
    else
    {
        QString path = CDC::getInstance()->configFile->value("/settings/chainPath").toString();
        QDir dir(path);
        QDir dir2(path + "/wallets");
        QDir dir3(path + "/chain");

        if( !dir.exists() || ( !dir2.exists() && !dir3.exists())  )  // 如果数据文件被删除了
        {
            selectWalletPathWidget = new SelectWalletPathWidget(this);
            selectWalletPathWidget->setAttribute(Qt::WA_DeleteOnClose);
            selectWalletPathWidget->move(0,0);
            connect( selectWalletPathWidget,SIGNAL(enter()),this,SLOT(showWaittingForSyncWidget()));
            connect( selectWalletPathWidget,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( selectWalletPathWidget,SIGNAL(closeWallet()),qApp,SLOT(quit()));
            connect( selectWalletPathWidget,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
            connect( selectWalletPathWidget,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));

            selectWalletPathWidget->show();

            CDC::getInstance()->configFile->clear();
            CDC::getInstance()->configFile->setValue("/settings/lockMinutes",5);
            CDC::getInstance()->lockMinutes     = 5;
            CDC::getInstance()->configFile->setValue("/settings/notAutoLock",false);
            CDC::getInstance()->notProduce      =  true;
            CDC::getInstance()->configFile->setValue("/settings/language","Simplified Chinese");
            CDC::getInstance()->language = "Simplified Chinese";
            CDC::getInstance()->minimizeToTray  = false;
            CDC::getInstance()->configFile->setValue("/settings/minimizeToTray",false);
            CDC::getInstance()->closeToMinimize = false;
            CDC::getInstance()->configFile->setValue("/settings/closeToMinimize",false);

        }
        else
        {
            waitingForSync = new WaitingForSync(this);
            waitingForSync->setAttribute(Qt::WA_DeleteOnClose);
            waitingForSync->move(0,0);
            connect( waitingForSync,SIGNAL(sync()), this, SLOT(syncFinished()));
            connect( waitingForSync,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( waitingForSync,SIGNAL(tray()),this,SLOT(hide()));
            connect( waitingForSync,SIGNAL(closeWallet()),qApp,SLOT(quit()));
            connect( waitingForSync,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
            connect( waitingForSync,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
            waitingForSync->show();

            CDC::getInstance()->appDataPath = CDC::getInstance()->configFile->value("/settings/chainPath").toString();

            QStringList strList;
            strList << "--data-dir" << CDC::getInstance()->configFile->value("/settings/chainPath").toString()
                    << "--rpcuser" << "a" << "--rpcpassword" << "b" << "--rpcport" << QString::number(RPC_PORT) << "--server";
            if( CDC::getInstance()->configFile->value("/settings/resyncNextTime",false).toBool())
            {
                strList << "--resync-blockchain";

                QFile file(CDC::getInstance()->configFile->value("/settings/chainPath").toString() + "\\config.json");
                if(file.exists())   file.remove();
            }
            CDC::getInstance()->configFile->setValue("settings/resyncNextTime",false);
#ifdef WIN32
            CDC::getInstance()->startBlockChain(strList );

            if( CDC::getInstance()->proc->waitForStarted())
            {
                qDebug() << "launch cdc.exe succeeded";
            }
            else
            {
                qDebug() << "launch cdc.exe failed";
            }
#else
            CDC::getInstance()->proc->start("./cdc",strList );

            if( CDC::getInstance()->proc->waitForStarted())
            {
                qDebug() << "launch cdc succeeded";
            }
            else
            {
                qDebug() << "launch cdc failed";
            }
#endif

        }


    }
    mutexForConfigFile.unlock();


    this->setWindowIcon(QIcon(":/pic/cplpic/logo.png"));
    trayIcon = new QSystemTrayIcon(this);
    //放在托盘提示信息、托盘图标
    trayIcon ->setToolTip(QString(ASSET_NAME"Wallet ") + WALLET_VERSION);
    trayIcon ->setIcon(QIcon(":/pic/cplpic/logo.png"));
    //点击托盘执行的事件
    connect(trayIcon , SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();
    createTrayIconActions();
    createTrayIcon();


    DLOG_QT_WALLET_FUNCTION_END;
}

Frame::~Frame()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    qDebug() << "~Frame begin;";

    if(centralWidget)
    {
        delete centralWidget;
        centralWidget = NULL;
    }

	if (titleBar)
	{
		delete titleBar;
		titleBar = NULL;
	}

    if (timer)
	{
		delete timer;
		timer = NULL;
	}

	if (bottomBar)
	{
		delete bottomBar;
		bottomBar = NULL;
	}

    if (lockPage)
    {
        delete lockPage;
        lockPage = NULL;
    }

    if( waitingForSync)
    {
        delete waitingForSync;
        waitingForSync = NULL;
    }

    if( functionBar)
    {
        delete functionBar;
        functionBar = NULL;
    }


qDebug() << "~Frame end;";
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::alreadyLogin()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    titleBar = new TitleBar(this);
    titleBar->setGeometry(0,0,960,53);
    connect(titleBar,SIGNAL(minimum()),this,SLOT(showMinimized()));
    connect(titleBar,SIGNAL(closeWallet()),qApp,SLOT(quit()));
    connect(titleBar,SIGNAL(tray()),this,SLOT(hide()));
    connect(titleBar,SIGNAL(settingSaved()),this,SLOT(settingSaved()));
    connect(titleBar,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(titleBar,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(titleBar,SIGNAL(showAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    connect(titleBar,SIGNAL(lock()),this,SLOT(showLockPage()));


    titleBar->show();

    centralWidget = new QWidget(this);
    centralWidget->setGeometry(103,53,857,529);
    centralWidget->show();

    bottomBar = new BottomBar(this);
    bottomBar->move(103,540);
    bottomBar->show();

//    showBottomBarWidget = new ShowBottomBarWidget(centralWidget);
//    showBottomBarWidget->setGeometry(0,525,827,20);
//    connect(showBottomBarWidget, SIGNAL(showBottomBar()), bottomBar, SLOT(dynamicShow()) );
//    showBottomBarWidget->show();

    functionBar = new FunctionBar(this);
    functionBar->move(0,53);
    functionBar->show();
    connect(functionBar, SIGNAL(showMainPage()), this, SLOT( showMainPage()));
    connect(functionBar, SIGNAL(showAssetPage()), this, SLOT( showAssetPage()));
    connect(functionBar, SIGNAL(showTransferPage()), this, SLOT( showTransferPage()));
    connect(functionBar, SIGNAL(showMultiSigPage()), this, SLOT( showMultiSigPage()));
    connect(functionBar, SIGNAL(showContactPage()), this, SLOT( showContactPage()));

    getAccountInfo();

    mainPage = new MainPage(centralWidget);
    mainPage->setAttribute(Qt::WA_DeleteOnClose);
//    QTimer::singleShot(1000,mainPage,SLOT(show()));
    mainPage->show();
    currentPageNum = 0;
    connect(mainPage,SIGNAL(openAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    connect(mainPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(mainPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(mainPage,SIGNAL(showTransferPage(QString)),this,SLOT(showTransferPage(QString)));
    connect(mainPage,SIGNAL(newAccount(QString)),this,SLOT(newAccount(QString)));

    timer = new QTimer(this);               //  自动锁定
    connect(timer,SIGNAL(timeout()),this,SLOT(autoLock()));
    if(CDC::getInstance()->notProduce)
    {
        timer->start(CDC::getInstance()->lockMinutes * 60000);
    }

    startTimerForAutoRefresh();              // 自动刷新

    init();

    DLOG_QT_WALLET_FUNCTION_END;
}

QString toThousandFigure( int number)     // 转换为0001,0015  这种数字格式
{
    if( number <= 9999 && number >= 1000)
    {
        return QString::number(number);
    }

    if( number <= 999 && number >= 100)
    {
        return QString( "0" + QString::number(number));
    }

    if( number <= 99 && number >= 10)
    {
        return QString( "00" + QString::number(number));
    }

    if( number <= 9 && number >= 0)
    {
        return QString( "000" + QString::number(number));
    }
    return "99999";
}

void Frame::getAccountInfo()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    CDC::getInstance()->postRPC( toJsonFormat( "id_wallet_list_my_addresses", "wallet_list_my_addresses", QStringList() << ""));

    CDC::getInstance()->postRPC( toJsonFormat( "id_balance", "balance", QStringList() << ""));

    CDC::getInstance()->updateAllContractBalance();

    CDC::getInstance()->collectContracts();

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::showAccountPage(QString accountName)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    currentAccount = accountName;
    CDC::getInstance()->currentAccount = accountName;
    getAccountInfo();

    closeCurrentPage();
    accountPage = new AccountPage(accountName,centralWidget);
    accountPage->setAttribute(Qt::WA_DeleteOnClose);
    accountPage->show();
    currentPageNum = 1;
    connect(accountPage,SIGNAL(back()),this,SLOT(showMainPage()));
    connect(accountPage,SIGNAL(accountChanged(QString)),this,SLOT(showAccountPage(QString)));
    connect(accountPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(accountPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));

    functionBar->choosePage(1);
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::showTransferPage(QString accountName)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    closeCurrentPage();
    getAccountInfo();
    CDC::getInstance()->currentAccount = accountName;
    transferPage = new TransferPage(accountName,centralWidget);
    transferPage->setAttribute(Qt::WA_DeleteOnClose);
    connect(transferPage,SIGNAL(accountChanged(QString)),this,SLOT(showTransferPage(QString)));
    connect(transferPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(transferPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(transferPage,SIGNAL(showAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    transferPage->show();

    currentPageNum = 3;
    functionBar->choosePage(3);

    DLOG_QT_WALLET_FUNCTION_END;
}


void Frame::showLockPage()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    timer->stop();
    shadowWidgetShow();


    CDC::getInstance()->postRPC( toJsonFormat( "id_lock", "lock", QStringList() << "" ));
qDebug() << "lock ";
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::autoLock()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    timer->stop();

    CDC::getInstance()->postRPC( toJsonFormat( "id_lock", "lock", QStringList() << "" ));
qDebug() << "autolock ";
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::unlock()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    if( CDC::getInstance()->notProduce)
    {
        timer->start(CDC::getInstance()->lockMinutes * 60000);
    }
    centralWidget->show();
    bottomBar->show();
    titleBar->show();
    qDebug() << "lockPage " << lockPage;
    if( lockPage)
    {
        lockPage->close();
        lockPage = NULL;
    }

qDebug() << "unlock showCurrentDialog";
    CDC::getInstance()->showCurrentDialog();

    if( CDC::getInstance()->consoleWidget)
    {
        CDC::getInstance()->consoleWidget->showNormal();
        CDC::getInstance()->consoleWidget->activateWindow();
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::updateTimer()
{
    if( timer != NULL && lockPage == NULL)
    {
        if( CDC::getInstance()->notProduce)
        {
            timer->stop();
            timer->start(CDC::getInstance()->lockMinutes * 60000);
        }
    }
}

void Frame::settingSaved()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    if( !CDC::getInstance()->notProduce)
    {
        timer->stop();
    }
    else
    {
        timer->start( CDC::getInstance()->lockMinutes * 60000);
    }

    QString language = CDC::getInstance()->language;
    if( language.isEmpty())
    {
        setLanguage("Simplified Chinese");
    }
    else
    {
        setLanguage(language);
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::privateKeyImported()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    getAccountInfo();
    mainPage->updateAccountList();
    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();
     }
	 
#ifdef WIN32
    ::ClipCursor(&rtConfined);
#endif
}

void Frame::mouseMoveEvent(QMouseEvent *event)
{

    //若鼠标左键被按下
    if(mouse_press)
    {
        //鼠标相对于屏幕的位置
        QPoint move_pos = event->globalPos();

        //移动主窗体位置
        this->move(move_pos - move_point);
    }

}

void Frame::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
	
#ifdef WIN32
    ::ClipCursor(&rtDefault);
#endif
}

void Frame::refreshAccountInfo()
{
    needToRefresh = true;
    getAccountInfo();
}

void Frame::startTimerForAutoRefresh()
{
    if( timerForAutoRefresh != NULL)
    {
        timerForAutoRefresh->stop();
        delete timerForAutoRefresh;
    }

    timerForAutoRefresh = new QTimer(this);
    connect(timerForAutoRefresh,SIGNAL(timeout()),this,SLOT(autoRefresh()));
    timerForAutoRefresh->start(AUTO_REFRESH_TIME);
}

void Frame::syncFinished()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    waitingForSync->timer->stop();

    RpcThread* rpcThread = new RpcThread;
    connect(rpcThread,SIGNAL(finished()),rpcThread,SLOT(deleteLater()));
    rpcThread->setLogin("a","b");
    rpcThread->setWriteData( toJsonFormat( "id_open", "open", QStringList() << "wallet" ));
    rpcThread->start();

    CDC::getInstance()->initWorkerThreadManager();

    if( CDC::getInstance()->contactsFile == NULL)
    {
        CDC::getInstance()->getContactsFile();
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::quit()
{
    CDC::getInstance()->write("stop\n");

    QTimer* timerForSafeQuit = new QTimer(this);
    timerForSafeQuit->start(50);
    connect(timerForSafeQuit,SIGNAL(timeout()),this,SLOT(checkAlreadyQuit()));
}

void Frame::checkAlreadyQuit()
{

    if( CDC::getInstance()->proc->state() == QProcess::NotRunning)
    {
        qApp->quit();
    }
    else
    {
        quitTimerCount--;
        if( quitTimerCount < 0)
        {
            qApp->quit();
        }
    }
}

void Frame::closeCurrentPage()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    qDebug() << " closeCurrentPage :" << currentPageNum;

    switch (currentPageNum) {
    case 0:
        mainPage->close();
        mainPage = NULL;
        break;
    case 1:
        accountPage->close();
        accountPage = NULL;
        break;
    case 2:
        break;
    case 3:
        transferPage->close();
        transferPage = NULL;
        break;
    case 4:
        contactPage->close();
        contactPage = NULL;
        break;
    case 5:
        multiSigPage->close();
        multiSigPage = NULL;
        break;
    case 6:
        break;
    case 8:
        assetPage->close();
        assetPage = NULL;
        break;
    default:
        break;
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::refresh()
{
//    getAccountInfo();

    switch (currentPageNum) {
    case 0:
        mainPage->refresh();
        break;
    case 1:
        accountPage->refresh();
        break;
    case 2:

        break;
    case 3:
        transferPage->refresh();
        break;
    case 4:
        break;
    case 5:
//        showAccountPage(currentAccount);

        break;
    case 6:
        break;
    case 7:
//        showUpgradePage();
        break;
    case 8:
        break;
    default:
        break;
    }
}

void Frame::autoRefresh()
{
    getAccountInfo();

    bottomBar->refresh();

    switch (currentPageNum) {
    case 0:
        mainPage->refresh();
        break;
    case 1:
        if( lockPage == NULL)     // 锁定的时候 refresh会崩溃
        {
            accountPage->refresh();
        }
        break;
    case 2:
        break;
    case 3:
//        showTransferPage( transferPage->getCurrentAccount());
        transferPage->refresh();
        break;
    case 4:
        break;
    case 5:
//        showAccountPage(currentAccount);
//        if( lockPage == NULL)     // 锁定的时候 refresh会崩溃
//        {
//            accountPage->refresh();
//        }
        break;
    case 6:
        break;
    case 7:
//        showUpgradePage();
        break;
    case 8:
        break;
        break;
    default:
        break;
    }
}


void Frame::showMainPage()
{
    closeCurrentPage();
    getAccountInfo();

    mainPage = new MainPage(centralWidget);
    mainPage->setAttribute(Qt::WA_DeleteOnClose);
    mainPage->show();
    currentPageNum = 0;
    connect(mainPage,SIGNAL(openAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    connect(mainPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(mainPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
//    connect(mainPage,SIGNAL(refreshAccountInfo()),this,SLOT(refreshAccountInfo()));
    connect(mainPage,SIGNAL(showTransferPage(QString)),this,SLOT(showTransferPage(QString)));
    connect(mainPage,SIGNAL(newAccount(QString)),this,SLOT(newAccount(QString)));

}

void Frame::showAssetPage()
{
    closeCurrentPage();
    getAccountInfo();

    assetPage = new AssetPage(centralWidget);
    assetPage->setAttribute(Qt::WA_DeleteOnClose);
    assetPage->show();
    currentPageNum = 8;
}

void Frame::showTransferPage()
{
    closeCurrentPage();
    getAccountInfo();
    transferPage = new TransferPage(CDC::getInstance()->currentAccount,centralWidget);
    transferPage->setAttribute(Qt::WA_DeleteOnClose);
    connect(transferPage,SIGNAL(accountChanged(QString)),this,SLOT(showTransferPage(QString)));
    connect(transferPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(transferPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(transferPage,SIGNAL(showAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    transferPage->show();
    currentPageNum = 3;
    functionBar->choosePage(3);
}

void Frame::showMultiSigPage()
{
    closeCurrentPage();
    getAccountInfo();
    multiSigPage = new MultiSigPage(centralWidget);
    multiSigPage->setAttribute(Qt::WA_DeleteOnClose);
    multiSigPage->show();
    currentPageNum = 5;
}


void Frame::showContactPage()
{
    closeCurrentPage();    
    getAccountInfo();
    contactPage = new ContactPage(centralWidget);
    connect(contactPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(contactPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(contactPage,SIGNAL(gotoTransferPage(QString,QString)),this,SLOT(showTransferPageWithAddress(QString,QString)));
    contactPage->setAttribute(Qt::WA_DeleteOnClose);
    contactPage->show();
    currentPageNum = 4;
}

bool Frame::eventFilter(QObject* watched, QEvent* e)
{
    if( (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::KeyPress)  )
    {
        updateTimer();
    }

    // currentDialog 上的鼠标事件也会移动 frame 和 本身
    if( CDC::getInstance()->currentDialogVector.contains(  (QWidget*)watched) )
    {
        if( e->type() == QEvent::MouseMove)
        {
            mouseMoveEvent( (QMouseEvent*)e);
        }
        else if( e->type() == QEvent::MouseButtonPress)
        {
            mousePressEvent( (QMouseEvent*)e);
        }
        else if( e->type() == QEvent::MouseButtonRelease)
        {
            mouseReleaseEvent( (QMouseEvent*)e);
        }

        CDC::getInstance()->resetPosOfCurrentDialog();  // currentDialog 一起移动

        return false;
    }

    return false;
}

void Frame::shadowWidgetShow()
{
    qDebug() << "shadowWidgetShow";
    shadowWidget->raise();
    shadowWidget->show();
}

void Frame::shadowWidgetHide()
{
    qDebug() << "shadowWidgetHide";
    shadowWidget->hide();
}

void Frame::showTransferPageWithAddress(QString remark, QString address)
{
    closeCurrentPage();
    getAccountInfo();
    transferPage = new TransferPage(CDC::getInstance()->currentAccount,centralWidget);
    transferPage->setAttribute(Qt::WA_DeleteOnClose);
    transferPage->setAddress(address);
    transferPage->setContact(remark);
    connect(transferPage,SIGNAL(accountChanged(QString)),this,SLOT(showTransferPage(QString)));
    connect(transferPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect(transferPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
    connect(transferPage,SIGNAL(showAccountPage(QString)),this,SLOT(showAccountPage(QString)));
    transferPage->show();
    currentPageNum = 3;
    functionBar->choosePage(3);
}

void Frame::setLanguage(QString language)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    menuTranslator.load(QString(":/qm/qt_zh_cn"));
    translatorForTextBrowser.load(":/language/widgets.qm");

    if( language == "Simplified Chinese")
    {
        translator.load(":/language/wallet_simplified_Chinese.qm");
        QApplication::installTranslator(&menuTranslator);
        QApplication::installTranslator(&translatorForTextBrowser);
    }
    else if( language == "English")
    {
        translator.load(":/language/wallet_English.qm");
        QApplication::removeTranslator(&menuTranslator);
        QApplication::removeTranslator(&translatorForTextBrowser);
    }

    QApplication::installTranslator(&translator);


    if( titleBar != NULL)       // 已经登录
    {
        functionBar->retranslator();
        titleBar->retranslator();
        bottomBar->retranslator();
        shadowWidget->retranslator();

        switch (currentPageNum) {
        case 0:
            showMainPage();
            break;
        case 1:
//            mainPage->retranslator(language);
            showAccountPage( currentAccount);
            break;
        case 2:
            break;
        case 3:
//            transferPage->retranslator(language);
            showTransferPage(currentAccount);
            break;
        case 4:
//            contactPage->retranslator(language);
            showContactPage();
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
//            upgradePage->retranslator();
//            showUpgradePage(currentAccount);
            break;
        case 8:
            showAssetPage();
            break;
        default:
            break;
        }
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::showWaittingForSyncWidget()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    if( selectWalletPathWidget)
    {
        selectWalletPathWidget->close();
        selectWalletPathWidget = NULL;
    }

    waitingForSync = new WaitingForSync(this);
    waitingForSync->setAttribute(Qt::WA_DeleteOnClose);
    waitingForSync->move(0,0);
    connect( waitingForSync,SIGNAL(sync()), this, SLOT(syncFinished()));
    connect( waitingForSync,SIGNAL(minimum()),this,SLOT(showMinimized()));
    connect( waitingForSync,SIGNAL(tray()),this,SLOT(hide()));
    connect( waitingForSync,SIGNAL(closeWallet()),this,SLOT(quit()));
    connect( waitingForSync,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
    connect( waitingForSync,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));

    waitingForSync->show();

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::jsonDataUpdated(QString id)
{
    if( id == "id_open")
    {
        waitingForSync->close();
        waitingForSync = NULL;

        activateWindow();
        QString result = CDC::getInstance()->jsonDataValue(id);

        if( result == "\"result\":null")
        {
            normalLogin = new NormalLogin(this);
            firstLogin = NULL;
            normalLogin->setGeometry(0,0,960,580);
            normalLogin->show();
            connect( normalLogin,SIGNAL(login()), this, SLOT(alreadyLogin()));
            connect( normalLogin,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( normalLogin,SIGNAL(closeWallet()),this,SLOT(quit()));
            connect( normalLogin,SIGNAL(tray()),this,SLOT(hide()));
            connect( normalLogin,SIGNAL(showShadowWidget()), this, SLOT(shadowWidgetShow()));
            connect( normalLogin,SIGNAL(hideShadowWidget()), this,SLOT(shadowWidgetHide()));
        }
        else if( result.startsWith("\"error\":{\"message\":\"wallet does not exist\""))
        {
            firstLogin = new FirstLogin(this);
            normalLogin = NULL;
            firstLogin->setGeometry(0,0,960,580);
            firstLogin->show();
            connect( firstLogin,SIGNAL(login()), this, SLOT(alreadyLogin()));
            connect( firstLogin,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( firstLogin,SIGNAL(closeWallet()),this,SLOT(quit()));
            connect( firstLogin,SIGNAL(tray()),this,SLOT(hide()));
            connect( firstLogin,SIGNAL(showShadowWidget()), this, SLOT(shadowWidgetShow()));
            connect( firstLogin,SIGNAL(hideShadowWidget()), this,SLOT(shadowWidgetHide()));
        }
        else
        {
            // 其他情况视为钱包数据损坏
            QDir dir(CDC::getInstance()->appDataPath + "/wallets/wallet");
            if(!dir.exists())   return;
            QFileInfoList fileInfos = dir.entryInfoList( QStringList() << "*.log");
            for(int i= 0; i < fileInfos.size(); i++)
            {
                QString path = fileInfos.at(i).absoluteFilePath();
                QFile file(path);
                file.rename(file.fileName() + ".bak");
            }

            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);


            CommonDialog commonDialog(CommonDialog::OkOnly);
//            commonDialog.setText(QString::fromLocal8Bit("钱包数据损坏，请重新启动: ") + errorMessage );
            commonDialog.setText(tr("Wallet data error: ") + errorMessage );
            commonDialog.pop();

            qApp->quit();
        }

        return;
    }

    if( id == "id_wallet_list_my_addresses")
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        QStringList accountList;

        QStringList strList = result.split("},{");

        foreach (QString str, strList)
        {
            int pos = str.indexOf("\"name\":\"") + 8;
            if( pos == 7)  break;   // 如果还没有账号

            accountList += str.mid(pos, str.indexOf("\",", pos) - pos);
        }

        mutexForConfigFile.lock();
        CDC::getInstance()->configFile->beginGroup("/accountInfo");
        QStringList keys = CDC::getInstance()->configFile->childKeys();
        CDC::getInstance()->configFile->endGroup();


        foreach (QString key, keys)
        {
            QString addName = CDC::getInstance()->configFile->value("/accountInfo/" + key).toString();
            if( !accountList.contains( addName) && !addName.isEmpty())
            {
                // 如果config记录的账户钱包中没有 则清除config文件中记录以及内存中各map
                CDC::getInstance()->deleteAccountInConfigFile( addName);
                CDC::getInstance()->addressMapRemove( addName);
                CDC::getInstance()->balanceMapRemove( addName);
                CDC::getInstance()->registerMapRemove( addName);
            }
        }

        CDC::getInstance()->configFile->beginGroup("/accountInfo");
        keys = CDC::getInstance()->configFile->childKeys();

        foreach (QString key, keys)
        {
            QString addName = CDC::getInstance()->configFile->value(key).toString();
            CDC::getInstance()->balanceMapInsert(addName, CDC::getInstance()->getBalance( addName));
            CDC::getInstance()->registerMapInsert(addName, CDC::getInstance()->getRegisterTime( addName));
            CDC::getInstance()->addressMapInsert(addName, CDC::getInstance()->getAddress (addName));
            accountList.removeAll( addName);     // 如果钱包中有账号 config中未记录， 保留在accountList里
        }
        CDC::getInstance()->configFile->endGroup();

        if( !accountList.isEmpty())    // 把config中没有记录的账号写入config中
        {
            for( int i = 0; i < accountList.size(); i++)
            {
                QString addName = accountList.at(i);
                mutexForBalanceMap.lock();
                QString accountName = QString(QString::fromLocal8Bit("账户") + toThousandFigure(CDC::getInstance()->balanceMap.size() + 1));
                mutexForBalanceMap.unlock();
                CDC::getInstance()->configFile->setValue("/accountInfo/" + accountName, addName);
                CDC::getInstance()->balanceMapInsert(addName, CDC::getInstance()->getBalance( addName));
                CDC::getInstance()->registerMapInsert(addName, CDC::getInstance()->getRegisterTime( addName));
                CDC::getInstance()->addressMapInsert(addName, CDC::getInstance()->getAddress ( addName));
            }
        }
        mutexForConfigFile.unlock();

        if( needToRefresh)
        {
            refresh();
            needToRefresh = false;
        }

        return;
    }


    if( id == "id_lock")
    {
        if( lockPage )
        {
            qDebug() << "already exist a lockpage";
            return;
        }

        QString result = CDC::getInstance()->jsonDataValue(id);
        if( result == "\"result\":null")
        {
            shadowWidgetHide();

            lockPage = new LockPage(this);
            lockPage->setAttribute(Qt::WA_DeleteOnClose);
            lockPage->setGeometry(0,0,960,580);
            connect( lockPage,SIGNAL(unlock()),this,SLOT(unlock()));
            connect( lockPage,SIGNAL(minimum()),this,SLOT(showMinimized()));
            connect( lockPage,SIGNAL(closeWallet()),this,SLOT(quit()));
            connect( lockPage,SIGNAL(tray()),this,SLOT(hide()));
            connect( lockPage,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
            connect( lockPage,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
            lockPage->show();

            CDC::getInstance()->hideCurrentDialog();

            if( CDC::getInstance()->consoleWidget)
            {
                CDC::getInstance()->consoleWidget->hide();
            }

        }
        return;
    }


    if( id.mid(0,37) == "id_wallet_get_account_public_address-" )
    {
        return;
    }

    if( id == "id_blockchain_list_assets")
    {
        CDC::getInstance()->parseAssetInfo();

        updateAssets();

        CDC::getInstance()->updateAllToken();

        return;
    }

    if( id == "id_balance")
    {
        CDC::getInstance()->parseBalance();
        return;
    }

    if( id.startsWith( "id_contract_call_offline_state+") )
    {
        QString result = CDC::getInstance()->jsonDataValue(id);
        if( !result.startsWith( "\"error\":") && result != "\"result\":\"NOT_INITED\"")
        {
            QString contractAddress = id.mid(31);
            CDC::getInstance()->ERC20TokenInfoMap[contractAddress].contractAddress = contractAddress;
            CDC::getInstance()->configFile->setValue("/AddedContractToken/" + contractAddress,1);
            CDC::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_+tokenName+" + contractAddress, "contract_call_offline", QStringList() << contractAddress
                                                       << CDC::getInstance()->addressMap.keys().at(0) << "tokenName" << ""
                                                       ));

            CDC::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_+precision+" + contractAddress, "contract_call_offline", QStringList() << contractAddress
                                                       << CDC::getInstance()->addressMap.keys().at(0) << "precision" << ""
                                                       ));

            CDC::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_+admin+" + contractAddress, "contract_call_offline", QStringList() << contractAddress
                                                       << CDC::getInstance()->addressMap.keys().at(0) << "admin" << ""
                                                       ));

            CDC::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_+tokenSymbol+" + contractAddress, "contract_call_offline", QStringList() << contractAddress
                                                       << CDC::getInstance()->addressMap.keys().at(0) << "tokenSymbol" << ""
                                                       ));

            CDC::getInstance()->postRPC( toJsonFormat( "id_contract_call_offline_+totalSupply+" + contractAddress, "contract_call_offline", QStringList() << contractAddress
                                                       << CDC::getInstance()->addressMap.keys().at(0) << "totalSupply" << ""
                                                       ));
        }

        return;
    }

    if( id.startsWith( "id_contract_call_offline_+") )
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            int pos = 26;
            QString func = id.mid( pos, id.indexOf("+" , pos) - pos);
            QString contractAddress = id.mid(id.indexOf("+" , pos) + 1);

            if( func == "tokenName")
            {
                QString tokenName = result.mid(9);
                tokenName.remove('\"');
                CDC::getInstance()->ERC20TokenInfoMap[contractAddress].name = tokenName;
            }
            else if( func == "precision")
            {
                QString precision = result.mid(9);
                precision.remove('\"');
                CDC::getInstance()->ERC20TokenInfoMap[contractAddress].precision = precision.toDouble();
            }
            else if( func == "admin")
            {
                QString admin = result.mid(9);
                admin.remove('\"');
                CDC::getInstance()->ERC20TokenInfoMap[contractAddress].admin = admin;
            }
            else if( func == "tokenSymbol")
            {
                QString tokenSymbol = result.mid(9);
                tokenSymbol.remove('\"');
                CDC::getInstance()->ERC20TokenInfoMap[contractAddress].symbol = tokenSymbol;
            }
            else if( func == "totalSupply")
            {
                QString totalSupply = result.mid(9);
                totalSupply.remove('\"');
                CDC::getInstance()->ERC20TokenInfoMap[contractAddress].totalSupply = totalSupply.toDouble();
            }
        }

        return;
    }

    if( id.startsWith( "id_contract_call_offline_balanceOf+") )
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        int pos = QString("id_contract_call_offline_balanceOf+").size();
        QString contractAddress = id.mid( pos, id.indexOf("+" , pos) - pos);
        QString accountAddress = id.mid(id.indexOf("+" , pos) + 1);

        if( result.startsWith("\"result\":"))
        {
            result = result.mid(9);
            result.remove('\"');
            double balance = result.toDouble();

            ContractBalanceMap map;
            if( CDC::getInstance()->accountContractBalanceMap.contains(accountAddress))
            {
                map = CDC::getInstance()->accountContractBalanceMap[accountAddress];
            }
            map.insert(contractAddress,balance);
            CDC::getInstance()->accountContractBalanceMap.insert(accountAddress,map);
        }


        return;
    }

    if( id.startsWith( "id_blockchain_list_contract_transaction_history+") )
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            int pos = QString("id_blockchain_list_contract_transaction_history+").size();
            QString contractAddress = id.mid( pos, id.indexOf("+" , pos) - pos);
            QString toBlockHeight = id.mid(id.indexOf("+" , pos) + 1);
            if( !CDC::getInstance()->ERC20TokenInfoMap.contains(contractAddress)) return;
            CDC::getInstance()->ERC20TokenInfoMap[contractAddress].collectedBlockHeight = toBlockHeight.toInt();

            result.prepend("{");
            result.append("}");

            QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
            QByteArray ba = utfCodec->fromUnicode(result);

            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);
            if(json_error.error == QJsonParseError::NoError)
            {
                if( parse_doucment.isObject())
                {
                    QJsonObject jsonObject = parse_doucment.object();
                    if( jsonObject.contains("result"))
                    {
                        QJsonValue resultValue = jsonObject.take("result");
                        if( resultValue.isArray())
                        {
                            QJsonArray resultArray = resultValue.toArray();
                            for( int i = 0; i < resultArray.size(); i++)
                            {
                                QJsonObject object          = resultArray.at(i).toObject();
                                QString trxId               = object.take("result_trx_id").toString();
                                CDC::getInstance()->postRPC( toJsonFormat( "id_blockchain_get_pretty_contract_transaction" + trxId, "blockchain_get_pretty_contract_transaction",
                                                                             QStringList() << trxId
                                                                             ));
                            }
                        }
                    }
                }

            }
        }
        return;
    }

    if( id.startsWith( "id_blockchain_get_pretty_contract_transaction") )
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            result.prepend("{");
            result.append("}");

            QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
            QByteArray ba = utfCodec->fromUnicode(result);

            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);
            if(json_error.error == QJsonParseError::NoError)
            {
                if( parse_doucment.isObject())
                {
                    QJsonObject jsonObject = parse_doucment.object();
                    if( jsonObject.contains("result"))
                    {
                        ContractTransaction contractTransaction;
                        QJsonObject object = jsonObject.take("result").toObject();
                        contractTransaction.trxId = object.take("result_trx_id").toString();
                        contractTransaction.blockNum = object.take("block_num").toInt();
                        contractTransaction.timeStamp   = object.take("timestamp").toString();

                        QJsonObject object2 = object.take("to_contract_ledger_entry").toObject();
                        contractTransaction.fromAddress = object2.take("from_account").toString();
                        contractTransaction.contractAddress = object2.take("to_account").toString();
                        QJsonValue feeValue = object2.take("fee").toObject().take("amount");
                        if( feeValue.isString())
                        {
                            contractTransaction.fee = feeValue.toString().toULongLong();
                        }
                        else
                        {
                            contractTransaction.fee = QString::number(feeValue.toDouble(),'g',10).toULongLong();
                        }

                        QJsonArray array = object.take("reserved").toArray();
                        QStringList params = array.at(1).toString().split(",");
                        if(params.size() != 2)  return;
                        contractTransaction.toAddress = params.at(0);
                        contractTransaction.amount = params.at(1).toULongLong();

                        if( CDC::getInstance()->isMyAddress(contractTransaction.fromAddress))
                        {
                            QString key = contractTransaction.fromAddress + "-" + contractTransaction.contractAddress;
                            CDC::getInstance()->accountContractTransactionMap[key].append(contractTransaction);
                        }

                        if( CDC::getInstance()->isMyAddress(contractTransaction.toAddress) && contractTransaction.fromAddress != contractTransaction.toAddress)
                        {
                            QString key = contractTransaction.toAddress + "-" + contractTransaction.contractAddress;
                            CDC::getInstance()->accountContractTransactionMap[key].append(contractTransaction);
                        }

                    }
                }

            }

        }
        return;
    }

    if( id == "id_wallet_get_transaction_fee" )
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            int pos = result.indexOf("\"amount\":") + 9;
            QString amount = result.mid(pos, result.indexOf(",", pos) - pos);
            amount.remove("\"");
            qDebug() << "wallet_get_transaction_fee: " << amount;
            CDC::getInstance()->transactionFee = amount.toLongLong();

            if( currentPageNum == 3 && transferPage != NULL)
            {
                transferPage->updateTransactionFee();
            }
        }
        return;
    }
}

void Frame::closeEvent(QCloseEvent *e)
{

    hide();
    e->ignore();

}

void Frame::iconIsActived(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        //点击托盘显示窗口
        case QSystemTrayIcon::Trigger:
        {
            showNormalAndActive();
            break;
        }

        default:
            break;
    }
}

void Frame::createTrayIconActions()
{
     minimizeAction = new QAction(tr("Minimize"), this);
     connect(minimizeAction, SIGNAL(triggered()), this, SLOT(showMinimized()));

     restoreAction = new QAction(tr("Restore"), this);
     connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormalAndActive()));

     quitAction = new QAction(tr("Quit"), this);
     connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));
}

void Frame::createTrayIcon()
{
     trayIconMenu = new QMenu(this);
     trayIconMenu->addAction(minimizeAction);
     trayIconMenu->addAction(restoreAction);
     trayIconMenu->addSeparator();
     trayIconMenu->addAction(quitAction);
     trayIcon->setContextMenu(trayIconMenu);
}

void Frame::showNormalAndActive()
{
    showNormal();
    activateWindow();

    if( CDC::getInstance()->consoleWidget)
    {
        CDC::getInstance()->consoleWidget->showNormal();
        CDC::getInstance()->consoleWidget->activateWindow();
    }
}


// 提前载入，防止切换到别的界面显示不出来
void Frame::init()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

    CDC::getInstance()->postRPC( toJsonFormat( "id_blockchain_list_pending_transactions", "blockchain_list_pending_transactions", QStringList() << "" ));

    CDC::getInstance()->postRPC(  toJsonFormat( "id_wallet_set_transaction_scanning", "wallet_set_transaction_scanning", QStringList() << "true" ) );

    CDC::getInstance()->postRPC( toJsonFormat( "id_blockchain_list_assets", "blockchain_list_assets", QStringList() << ""));

    CDC::getInstance()->postRPC( toJsonFormat( "id_wallet_get_transaction_fee", "wallet_get_transaction_fee", QStringList() << "" ));

    DLOG_QT_WALLET_FUNCTION_END;
}

void Frame::scan()
{
    CDC::getInstance()->postRPC( toJsonFormat( "id_scan", "scan", QStringList() << "0"));
}

void Frame::newAccount(QString name)
{
    getAccountInfo();
}

void Frame::updateAssets()
{
    if( assetPage != NULL)
    {
        assetPage->updateAssetInfo();
        assetPage->updateMyAsset();
        return;
    }
}

