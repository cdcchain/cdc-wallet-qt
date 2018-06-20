// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "editremarkdialog.h"
#include "ui_editremarkdialog.h"
#include "blockchain.h"
#include "pubic_define.h"

EditRemarkDialog::EditRemarkDialog(QString remark, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditRemarkDialog)
{
    ui->setupUi(this);

    setParent(CDC::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(246, 246, 246);border:1px groove rgb(180,180,180);}");


    ui->remarkLineEdit->setText( remark);
    ui->remarkLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->remarkLineEdit->setTextMargins(8,0,0,0);
    ui->okBtn->setStyleSheet("QToolButton{background-color:rgb("STR_BUTTON_COLOR");color:#ffffff;border:none;border-radius:3px;}QToolButton:hover{background-color:rgb("STR_BUTTON_COLOR");}");
    ui->remarkLineEdit->setFocus();
}

EditRemarkDialog::~EditRemarkDialog()
{
    delete ui;
}

void EditRemarkDialog::on_okBtn_clicked()
{
    close();
}

QString EditRemarkDialog::pop()
{
    move(0,0);
    exec();

    return ui->remarkLineEdit->text();
}

void EditRemarkDialog::on_remarkLineEdit_textChanged(const QString &arg1)
{
    QString remark = arg1;
    if( remark.contains("=") || remark.contains(";") || remark.contains(" ") )
    {
        remark.remove("=");
        remark.remove(";");
        remark.remove(" ");
        ui->remarkLineEdit->setText( remark);
    }
}
