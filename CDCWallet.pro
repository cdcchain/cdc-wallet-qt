#-------------------------------------------------
#
# Project created by QtCreator 2015-07-02T14:17:06
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CDCWallet
TEMPLATE = app

win32{
    QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
    LIBS += -lDbgHelp
    LIBS += User32.Lib
    LIBS += -limm32
}

macx{
    ICON = logo.icns
QMAKE_MAC_SDK = macosx10.12
}

LIBS += -L$$PWD -lqrencode


SOURCES += main.cpp\
        firstlogin.cpp \
    normallogin.cpp \
    frame.cpp \
    mainpage.cpp \
    accountpage.cpp \
    transferpage.cpp \
    bottombar.cpp \
    setdialog.cpp \
    lockpage.cpp \
    consoledialog.cpp \
    titlebar.cpp \
    debug_log.cpp \
    contactdialog.cpp \
    waitingforsync.cpp \
    remarkdialog.cpp \
    functionbar.cpp \
    contactpage.cpp \
    singlecontactwidget.cpp \
    commondialog.cpp \
    showcontentdialog.cpp \
    namedialog.cpp \
    deleteaccountdialog.cpp \
    transferconfirmdialog.cpp \
    commontip.cpp \
    searchoptionwidget.cpp \
    rpcthread.cpp \
    addnodedialog.cpp \
    editremarkdialog.cpp \
    addcontactdialog.cpp \
    exportdialog.cpp \
    importdialog.cpp \
    ipcellwidget.cpp \
    incomecellwidget.cpp \
    control/myprogressbar.cpp \
    workerthread.cpp \
    workerthreadmanager.cpp \
    control/showbottombarwidget.cpp \
    control/rightclickmenudialog.cpp \
    control/chooseaddaccountdialog.cpp \
    dialog/renamedialog.cpp \
    extra/dynamicmove.cpp \
    control/remarkcellwidget.cpp \
    control/shadowwidget.cpp \
    assetpage.cpp \
    dialog/assetissuedialog.cpp \
    selectwalletpathwidget.cpp \
    dialog/addtokendialog.cpp \
    consolewidget.cpp \
    multisigpage.cpp \
    control/qrcodewidget.cpp \
    control/qrcodedialog.cpp \
    dialog/exportsetpwddialog.cpp \
    AES/aes.cpp \
    AES/aesencryptor.cpp \
    dialog/importenterpwddialog.cpp \
    blockchain.cpp

HEADERS  += firstlogin.h \
    normallogin.h \
    frame.h \
    mainpage.h \
    accountpage.h \
    transferpage.h \
    bottombar.h \
    setdialog.h \
    lockpage.h \
    consoledialog.h \
    debug_log.h \
    titlebar.h \
    contactdialog.h \
    waitingforsync.h \
    remarkdialog.h \
    functionbar.h \
    contactpage.h \
    singlecontactwidget.h \
    commondialog.h \
    showcontentdialog.h \
    namedialog.h \
    deleteaccountdialog.h \
    transferconfirmdialog.h \
    commontip.h \
    searchoptionwidget.h \
    rpcthread.h \
    addnodedialog.h \
    editremarkdialog.h \
    addcontactdialog.h \
    exportdialog.h \
    importdialog.h \
    ipcellwidget.h \
    incomecellwidget.h \
    control/myprogressbar.h \
    workerthread.h \
    workerthreadmanager.h \
    control/showbottombarwidget.h \
    control/rightclickmenudialog.h \
    control/chooseaddaccountdialog.h \
    dialog/renamedialog.h \
    extra/dynamicmove.h \
    control/remarkcellwidget.h \
    control/shadowwidget.h \
    assetpage.h \
    dialog/assetissuedialog.h \
    selectwalletpathwidget.h \
    dialog/addtokendialog.h \
    consolewidget.h \
    multisigpage.h \
    control/qrcodewidget.h \
    control/qrcodedialog.h \
    dialog/exportsetpwddialog.h \
    AES/aes.h \
    AES/aesencryptor.h \
    dialog/importenterpwddialog.h \
    blockchain.h

FORMS    += firstlogin.ui \
    normallogin.ui \
    mainpage.ui \
    accountpage.ui \
    transferpage.ui \
    bottombar.ui \
    setdialog.ui \
    lockpage.ui \
    consoledialog.ui \
    titlebar.ui \
    contactdialog.ui \
    waitingforsync.ui \
    remarkdialog.ui \
    functionbar.ui \
    contactpage.ui \
    singlecontactwidget.ui \
    commondialog.ui \
    showcontentdialog.ui \
    namedialog.ui \
    deleteaccountdialog.ui \
    transferconfirmdialog.ui \
    commontip.ui \
    searchoptionwidget.ui \
    addnodedialog.ui \
    editremarkdialog.ui \
    addcontactdialog.ui \
    exportdialog.ui \
    importdialog.ui \
    ipcellwidget.ui \
    incomecellwidget.ui \
    control/accountdetailwidget.ui \
    control/rightclickmenudialog.ui \
    control/chooseaddaccountdialog.ui \
    dialog/renamedialog.ui \
    control/remarkcellwidget.ui \
    control/shadowwidget.ui \
    assetpage.ui \
    dialog/assetissuedialog.ui \
    selectwalletpathwidget.ui \
    dialog/addtokendialog.ui \
    consolewidget.ui \
    multisigpage.ui \
    control/qrcodedialog.ui \
    dialog/exportsetpwddialog.ui \
    dialog/importenterpwddialog.ui

win32{
    DISTFILES += logo.rc
    RC_FILE = logo.rc
}

RESOURCES += \
    fry.qrc

TRANSLATIONS +=   wallet_simplified_Chinese.ts  wallet_English.ts
