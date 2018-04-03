// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "exportsetpwddialog.h"
#include "ui_exportsetpwddialog.h"

#include <QDebug>

#include "blockchain.h"
#include "rpcthread.h"
#include "../commondialog.h"

ExportSetPwdDialog::ExportSetPwdDialog(QWidget *parent) :
    QDialog(parent),
    yesOrNO(false),
    ui(new Ui::ExportSetPwdDialog)
{
    ui->setupUi(this);

    setParent(CDC::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(246, 246, 246);border:1px groove rgb(180,180,180);}");


    connect( CDC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->pkPwdLineEdit->setMaxLength(14);
    ui->confirmPwdLineEdit->setMaxLength(14);

    ui->pkPwdLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->pkPwdLineEdit->setPlaceholderText( tr("Set the password from 8 to 14 digits"));
    ui->pkPwdLineEdit->setTextMargins(8,0,0,0);
    ui->pkPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pkPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->pkPwdLineEdit->setFocus();
    ui->pkPwdLineEdit->grabKeyboard();          // 不知道为什么可能输入不了  先这么处理
    ui->pkPwdLineEdit->releaseKeyboard();

    ui->confirmPwdLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->confirmPwdLineEdit->setTextMargins(8,0,0,0);
    ui->confirmPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->confirmPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);


    ui->okBtn->setEnabled(false);

}

ExportSetPwdDialog::~ExportSetPwdDialog()
{
    delete ui;
}

bool ExportSetPwdDialog::pop()
{
    move(0,0);
    exec();

    return yesOrNO;
}

void ExportSetPwdDialog::jsonDataUpdated(QString id)
{


}

void ExportSetPwdDialog::on_okBtn_clicked()
{
    if(ui->pkPwdLineEdit->text().isEmpty())     return;
    if(ui->pkPwdLineEdit->text() != ui->confirmPwdLineEdit->text())     return;
    pwd = ui->pkPwdLineEdit->text();
    yesOrNO = true;
    close();
}

void ExportSetPwdDialog::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

void ExportSetPwdDialog::on_pkPwdLineEdit_textEdited(const QString &arg1)
{
    checkOkBtn();
}

void ExportSetPwdDialog::on_confirmPwdLineEdit_textEdited(const QString &arg1)
{
    checkOkBtn();
}

void ExportSetPwdDialog::checkOkBtn()
{
    if( ui->pkPwdLineEdit->text() != ui->confirmPwdLineEdit->text())
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    if( ui->pkPwdLineEdit->text().size() < 8 || ui->pkPwdLineEdit->text().size() > 14)
    {
        ui->okBtn->setEnabled(false);
        return;
    }

    ui->okBtn->setEnabled(true);
}
