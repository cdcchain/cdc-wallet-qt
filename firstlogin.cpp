﻿// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <QDebug>
#include <QPainter>
#include <QKeyEvent>

#ifdef WIN32
#include <windows.h>
#endif

#include "firstlogin.h"
#include "ui_firstlogin.h"
#include "blockchain.h"
#include "debug_log.h"
#include "rpcthread.h"
#include "commondialog.h"
#include "pubic_define.h"

FirstLogin::FirstLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FirstLogin)
{
    ui->setupUi(this);

    connect(CDC::getInstance(),SIGNAL(jsonDataUpdated(QString)),this,SLOT(jsonDataUpdated(QString)));

    setAutoFillBackground(true);
    QPalette palette(this->palette());
    palette.setColor(QPalette::Background, QColor(MENU_BACKGROUND_COLOR));
    setPalette(palette);

    ui->pwdLineEdit->setStyleSheet("color:white;background:transparent;border-color:white;border-width:0px 0px 1px 0px;border-style:outset;");
    ui->pwdLineEdit2->setStyleSheet("color:white;background:transparent;border-color:white;border-width:0px 0px 1px 0px;border-style:outset;");
    ui->pwdLineEdit->setMaxLength(14);
    ui->pwdLineEdit2->setMaxLength(14);
#ifdef TARGET_OS_MAC
    ui->pwdLineEdit->setAttribute(Qt::WA_MacShowFocusRect,false);
    ui->pwdLineEdit2->setAttribute(Qt::WA_MacShowFocusRect,false);
#endif
    ui->pwdLineEdit->setFocus();
    ui->pwdLineEdit->installEventFilter(this);
    ui->pwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit2->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->pwdLineEdit2->setAttribute(Qt::WA_InputMethodEnabled, false);


    QFont font = ui->pwdLabel->font();
    font.setPixelSize(12);
    ui->pwdLabel->setFont(font);
    ui->pwdLabel2->setFont(font);

    font = ui->tipLabel->font();
    font.setPixelSize(12);
    ui->tipLabel->setFont(font);

    ui->minBtn->setStyleSheet("QToolButton{background-image:url(:/pic/cplpic/min.png);background-repeat: no-repeat;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/pic/cplpic/close.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->createBtn->setStyleSheet("color:white;border:1px solid rgb("STR_BUTTON_COLOR");background-color:rgb("STR_BUTTON_COLOR");");

    ui->correctLabel->setPixmap( QPixmap(":/pic/pic2/correct2.png"));
    ui->correctLabel2->setPixmap( QPixmap(":/pic/pic2/correct2.png"));
    ui->correctLabel->hide();
    ui->correctLabel2->hide();

    ui->titleBg->setStyleSheet("background-image: url(:/pic/cplpic/titleBg.png);");
    ui->titleBg->setGeometry(0,0,960,76);
    ui->logo->setStyleSheet("background-image: url(:/pic/cplpic/logo.png);");
    ui->logo->setGeometry(14,14,102,48);
    ui->version->setStyleSheet("color: white;");
    ui->version->setText("Wallet V"WALLET_VERSION);
    ui->walletIcon->setStyleSheet("background-image: url(:/pic/cplpic/walletIcon.png);background-repeat: no-repeat;");

#ifdef WIN32
    if( GetKeyState(VK_CAPITAL) )
    {
        ui->tipLabel->setText( tr("Caps lock opened!") );
    }
#endif

}

FirstLogin::~FirstLogin()
{
    delete ui;

}

void FirstLogin::on_createBtn_clicked()
{

    if( ui->pwdLineEdit->text().isEmpty() || ui->pwdLineEdit2->text().isEmpty() )
    {
        ui->tipLabel->setText(tr("Empty!"));
        return;
    }

    if( ui->pwdLineEdit->text() == ui->pwdLineEdit2->text())
    {
        if( ui->pwdLineEdit->text().size() < 8)
        {
            ui->tipLabel->setText(tr("Too short!"));
            return;
        }

        emit showShadowWidget();
        ui->pwdLineEdit->setEnabled(false);
        ui->pwdLineEdit2->setEnabled(false);

        CDC::getInstance()->postRPC( toJsonFormat( "id_create", "create", QStringList() << "wallet" << ui->pwdLineEdit->text() ));
    }
    else
    {
        ui->tipLabel->setText(tr("not consistent!"));
        return;
    }

}


void FirstLogin::paintEvent(QPaintEvent *)
{
}



void FirstLogin::on_pwdLineEdit2_returnPressed()
{
    on_createBtn_clicked();
}

void FirstLogin::on_minBtn_clicked()
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

void FirstLogin::on_closeBtn_clicked()
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

bool FirstLogin::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == ui->pwdLineEdit)
    {
        if(e->type() == QEvent::FocusIn)
        {
            ui->tipLabel2->show();
        }
        else if( e->type() == QEvent::FocusOut)
        {
            ui->tipLabel2->hide();
        }
    }

    return QWidget::eventFilter(watched,e);

}

void FirstLogin::on_pwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit->setText( ui->pwdLineEdit->text().remove(' '));
        return;
    }

    ui->tipLabel->clear();

    if( arg1.size() > 7 )
    {
        ui->correctLabel->show();

        if( ui->pwdLineEdit->text() == ui->pwdLineEdit2->text() )
        {
            ui->correctLabel2->show();
        }
        else
        {
            ui->correctLabel2->hide();
        }
    }
    else
    {
        ui->correctLabel->hide();
        ui->correctLabel2->hide();
    }

}

void FirstLogin::on_pwdLineEdit2_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit2->setText( ui->pwdLineEdit2->text().remove(' '));
        return;
    }

    ui->tipLabel->clear();

    if( arg1.size() > 7 && ( ui->pwdLineEdit->text() == ui->pwdLineEdit2->text() ) )
    {
        ui->correctLabel2->show();
    }
    else
    {
        ui->correctLabel2->hide();
    }
}

void FirstLogin::jsonDataUpdated(QString id)
{
    if( id == "id_create")
    {
        QString result = CDC::getInstance()->jsonDataValue(id);
        qDebug() << "id_create" << result;
        if( result == "\"result\":null" )
        {
            CDC::getInstance()->postRPC( toJsonFormat( "id_unlock_firstlogin", "unlock", QStringList() << "99999999" << ui->pwdLineEdit->text() ));
qDebug() << "id_unlock_firstlogin";
            return;
        }
        else
        {
            emit hideShadowWidget();
            return;
        }

    }

    if( id == "id_unlock_firstlogin")
    {
        emit hideShadowWidget();
        ui->pwdLineEdit->setEnabled(true);
        ui->pwdLineEdit2->setEnabled(true);

        qDebug() << "id_unlock_firstlogin" << CDC::getInstance()->jsonDataValue(id);
        if( CDC::getInstance()->jsonDataValue(id) == "\"result\":null")
        {
            emit login();
            this->close();
        }
        return;
    }

}

void FirstLogin::keyPressEvent(QKeyEvent *e)
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
