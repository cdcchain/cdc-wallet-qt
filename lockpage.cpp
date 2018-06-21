// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <QFileInfo>

#ifdef WIN32
#include <windows.h>
#endif

#include "pubic_define.h"
#include "lockpage.h"
#include "ui_lockpage.h"
#include "blockchain.h"
#include "debug_log.h"
#include "rpcthread.h"
#include "commondialog.h"

LockPage::LockPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LockPage)
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;

    ui->setupUi(this);

    connect( CDC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    setAutoFillBackground(true);
    QPalette palette(this->palette());
    palette.setColor(QPalette::Background, QColor(MENU_BACKGROUND_COLOR));
    setPalette(palette);

    ui->pwdLineEdit->setStyleSheet("color:white;background:transparent;border-width:0;border-style:outset;");
#ifdef TARGET_OS_MAC
    ui->pwdLineEdit->setAttribute(Qt::WA_MacShowFocusRect,false);
#endif
    ui->pwdLineEdit->setFocus();
    ui->pwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->enterBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/arrow.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                "QToolButton:hover{background-image:url(:/pic/pic2/arrow_hover.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    ui->minBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/minimize2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                              "QToolButton:hover{background-image:url(:/pic/pic2/minimize_hover.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/close2.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}"
                                "QToolButton:hover{background-image:url(:/pic/pic2/close_hover.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

#ifdef WIN32
    if( GetKeyState(VK_CAPITAL) )
    {
        ui->tipLabel->setText( tr("Caps lock opened!") );
    }
#endif

	DLOG_QT_WALLET_FUNCTION_END;
}

LockPage::~LockPage()
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;
    delete ui;

	DLOG_QT_WALLET_FUNCTION_END;
}

void LockPage::on_enterBtn_clicked()
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;

    if( ui->pwdLineEdit->text().isEmpty() )
    {
        ui->tipLabel->setText( tr("Empty!") );
        return;
    }

    if( ui->pwdLineEdit->text().size() < 8)
    {
        ui->tipLabel->setText(tr("At least 8 letters!"));
        ui->pwdLineEdit->clear();
        return;
    }


    CDC::getInstance()->postRPC( toJsonFormat( "id_unlock_lockpage", "unlock", QStringList() << "99999999" << ui->pwdLineEdit->text() ));
 qDebug() << "id_unlock_lockpage" ;
    emit showShadowWidget();
    ui->pwdLineEdit->setEnabled(false);

	DLOG_QT_WALLET_FUNCTION_END;
}


void LockPage::on_pwdLineEdit_returnPressed()
{
	DLOG_QT_WALLET_FUNCTION_BEGIN;

    on_enterBtn_clicked();

	DLOG_QT_WALLET_FUNCTION_END;
}

void LockPage::on_minBtn_clicked()
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

void LockPage::on_closeBtn_clicked()
{
    if( CDC::getInstance()->closeToMinimize)
    {
        emit tray();
    }
    else
    {
        CommonDialog commonDialog(CommonDialog::OkAndCancel);
        commonDialog.setText( tr( "Sure to close CDC Wallet?"));
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

void LockPage::on_pwdLineEdit_textChanged(const QString &arg1)
{
    if( !arg1.isEmpty())
    {
        ui->tipLabel->clear();
    }

    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit->setText( ui->pwdLineEdit->text().remove(' '));
    }
}

void LockPage::jsonDataUpdated(QString id)
{
    if( id == "id_unlock_lockpage")
    {
        emit hideShadowWidget();
        ui->pwdLineEdit->clear();
        ui->pwdLineEdit->setEnabled(true);
        ui->pwdLineEdit->setFocus();

        QString  result = CDC::getInstance()->jsonDataValue(id);

        if( result == "\"result\":null")
        {
            emit unlock();
        }
        else if( result.mid(0,8) == "\"error\":")
        {
            ui->tipLabel->setText(tr("Wrong password!"));
            ui->pwdLineEdit->clear();
        }

        return;
    }

}

void LockPage::keyPressEvent(QKeyEvent *e)
{
#ifdef WIN32
    if( e->key() == Qt::Key_CapsLock)
    {
        if( GetKeyState(VK_CAPITAL) == -127 )  // 不知道为什么跟构造函数中同样的调用返回的short不一样
        {
            ui->tipLabel->setText( tr("Caps lock opened!") );
        }
        else
        {

            ui->tipLabel->setText( tr("Caps lock closed!") );
        }
    }
#endif

    QWidget::keyPressEvent(e);
}
