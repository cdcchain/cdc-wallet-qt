// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <QDebug>
#include "commondialog.h"
#include "ui_commondialog.h"
#include "debug_log.h"
#include "blockchain.h"
#include "pubic_define.h"

CommonDialog::CommonDialog(commonDialogType type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommonDialog)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);


    setParent(CDC::getInstance()->mainFrame);
    move(CDC::getInstance()->mainFrame->pos());

    CDC::getInstance()->appendCurrentDialogVector(this);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget{background-color:rgba(10, 10, 10,100);}");

    ui->containerWidget->setObjectName("containerwidget");
    setStyleSheet("#containerwidget{background-color: rgb(246, 246, 246);border:1px groove rgb(180,180,180);}");

    yesOrNO = false;

    ui->okBtn->setText(tr("Ok"));
    ui->okBtn->setStyleSheet(QTOOL_BUTTON_STYLE);
    ui->cancelBtn->setText(tr("Cancel"));
    ui->cancelBtn->setStyleSheet("QToolButton{background-color:#ffffff;color:#484848;border:1px solid rgb("STR_BUTTON_COLOR");border-radius:3px;}QToolButton:hover{color:rgb("STR_BUTTON_COLOR");}");

    if( type == OkAndCancel)
    {
    }
    else if( type == OkOnly)
    {
        ui->cancelBtn->hide();
        ui->okBtn->move(101,96);
    }
    else if( type == YesOrNo)
    {
        ui->okBtn->setText(tr("Yes"));
        ui->cancelBtn->setText(tr("No"));
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

CommonDialog::~CommonDialog()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    qDebug() << "CommonDialog  delete ";
    delete ui;
    CDC::getInstance()->removeCurrentDialogVector(this);
    DLOG_QT_WALLET_FUNCTION_END;
}

void CommonDialog::on_okBtn_clicked()
{
    yesOrNO = true;
    close();
}

void CommonDialog::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

bool CommonDialog::pop()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    exec();
    return yesOrNO;
}

void CommonDialog::setText(QString text)
{
    ui->textLabel->setText(text);
}
