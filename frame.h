// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef FRAME_H
#define FRAME_H
#include <QFrame>
#include <QWidget>
#include <QMap>
#include <QTranslator>
#include <QSystemTrayIcon>

#ifdef WIN32
#include "windows.h"
#endif

namespace Ui {
   class Frame;
}

class FirstLogin;
class NormalLogin;
class MainPage;
class AccountPage;
class TransferPage;
class BottomBar;
class LockPage;
class TitleBar;
class QMenu;
class WaitingForSync;
class FunctionBar;
class ContactPage;
class SelectWalletPathWidget;
class ShadowWidget;
class AssetPage;
class MultiSigPage;

class Frame:public QFrame
{
    Q_OBJECT
public:
    Frame();
    ~Frame();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

public slots:
    void refreshAccountInfo();
    void refresh();
    void autoRefresh();
    void shadowWidgetShow();
    void shadowWidgetHide();
    void setLanguage(QString);
    void syncFinished();

    void quit();
    void checkAlreadyQuit();

signals:
    void started();
    void delegateListUpdated();

private slots:
    void alreadyLogin();
    void showAccountPage(QString);
    void showTransferPage(QString);
    void showLockPage();
    void autoLock();
    void unlock();
    void updateTimer();
    void settingSaved();
    void privateKeyImported();

    void jsonDataUpdated(QString id);

    void showMainPage();
    void showAssetPage();
    void showTransferPage();
    void showMultiSigPage();
    void showContactPage();
    void showTransferPageWithAddress(QString,QString);
    void showWaittingForSyncWidget();

    void iconIsActived(QSystemTrayIcon::ActivationReason reason);
    void showNormalAndActive();

    void scan();

    void newAccount(QString name);

    void updateAssets();

private:
    bool mouse_press;
    QPoint move_point;
    SelectWalletPathWidget*   selectWalletPathWidget;
    FirstLogin* firstLogin;
    NormalLogin* normalLogin;
    MainPage*   mainPage;
    AccountPage* accountPage;
    AssetPage* assetPage;
    TransferPage* transferPage;
    BottomBar* bottomBar;
    QWidget* centralWidget;
    LockPage*  lockPage;
    QTimer* timer;
    TitleBar* titleBar;
    QString lastPage;
    QString currentAccount;
    WaitingForSync* waitingForSync;
    int currentPageNum;  //  0:mainPage   1:accountPage  2:  3:transferPage    4:contactPage  5:multiSigPage
                         //   6:   7:upgradePage   8: assetPage
    ShadowWidget* shadowWidget;
    QSystemTrayIcon* trayIcon;
    void createTrayIconActions();
    void createTrayIcon();
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QMenu *trayIconMenu;
//    ShowBottomBarWidget* showBottomBarWidget;

    int quitTimerCount;     // 如果超时 直接退出

#ifdef WIN32
    RECT rtConfined;   // 由于定义了 framelesswindowhint 为了不让鼠标拖动时能移到任务栏下
    RECT rtDefault;
#endif

    void  getAccountInfo();
    void startTimerForAutoRefresh();      // 自动刷新
    QTimer* timerForAutoRefresh;
    FunctionBar* functionBar;
    void closeCurrentPage();
    bool eventFilter(QObject *watched, QEvent *e);
    void closeEvent(QCloseEvent* e);
    void init();

    MultiSigPage* multiSigPage;
    ContactPage* contactPage;

    QTranslator translator;         //  选择语言
    QTranslator menuTranslator;     //  右键菜单语言
    QTranslator translatorForTextBrowser;   // QTextBrowser的右键菜单翻译

    bool needToRefresh;

};


#endif // FRAME_H
