// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "deleteaccountdialog.h"
#include "ui_deleteaccountdialog.h"
#include "blockchain.h"
#include "rpcthread.h"
#include "commondialog.h"
#include "pubic_define.h"

#include <QDebug>
#include <QFocusEvent>

DeleteAccountDialog::DeleteAccountDialog(QString name , QWidget *parent) :
    QDialog(parent),
    accountName(name),
    yesOrNo( false),
    ui(new Ui::DeleteAccountDialog)
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

    ui->pwdLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->pwdLineEdit->setPlaceholderText( tr("Login password"));
    ui->pwdLineEdit->setTextMargins(8,0,0,0);
    ui->pwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->okBtn->setStyleSheet("QToolButton{background-color:rgb("STR_BUTTON_COLOR");color:#ffffff;border:none;border-radius:3px;}"
                             "QToolButton:hover{background-color:rgb("STR_BUTTON_COLOR");}"
                             "QToolButton:disabled{background-color:#cecece;}");
    ui->okBtn->setText(tr("Ok"));
    ui->okBtn->setEnabled(false);
    ui->cancelBtn->setStyleSheet("QToolButton{background-color:#ffffff;color:#484848;border:1px solid rgb("STR_BUTTON_COLOR");border-radius:3px;}"
                                 "QToolButton:hover{color:rgb("STR_BUTTON_COLOR");}");
    ui->cancelBtn->setText(tr("Cancel"));
    ui->pwdLineEdit->setFocus();
}

DeleteAccountDialog::~DeleteAccountDialog()
{
    delete ui;
}

void DeleteAccountDialog::on_okBtn_clicked()
{

    if( ui->pwdLineEdit->text().isEmpty())
    {
        ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr("Please enter the password!") + "</font></body>" );
        return;
    }

    CDC::getInstance()->postRPC( toJsonFormat( "id_wallet_check_passphrase", "wallet_check_passphrase", QStringList() << ui->pwdLineEdit->text()
                                               ));
}

void DeleteAccountDialog::on_cancelBtn_clicked()
{
    close();
}

void DeleteAccountDialog::on_pwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->pwdLineEdit->setText( ui->pwdLineEdit->text().remove(' '));
        return;
    }

    if( arg1.isEmpty())
    {
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->okBtn->setEnabled(true);
    }
}

void DeleteAccountDialog::pop()
{
    ui->pwdLineEdit->grabKeyboard();

    move(0,0);
    exec();

}

void DeleteAccountDialog::jsonDataUpdated(QString id)
{
    if( id.startsWith( "id_wallet_check_passphrase") )
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        if( result == "\"result\":true")
        {
            CDC::getInstance()->postRPC( toJsonFormat( "id_wallet_account_delete", "wallet_account_delete", QStringList() << accountName
                                                       ));

        }
        else if( result == "\"result\":false")
        {
            ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr("Wrong password!") + "</font></body>" );
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            if( errorMessage == "password too short")
            {
                ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr("At least 8 letters!") + "</font></body>" );
            }
            else
            {
                ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + errorMessage + "</font></body>" );
            }
        }

        return;
    }

    if( id == "id_wallet_account_delete" )
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        if( result == "\"result\":true")
        {
            CDC::getInstance()->deleteAccountInConfigFile( accountName);
            CDC::getInstance()->addressMapRemove( accountName);
            CDC::getInstance()->balanceMapRemove( accountName);
            CDC::getInstance()->registerMapRemove( accountName);

            close();

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Account deleted."));
            commonDialog.pop();
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(tr("Delete account failed: ") + errorMessage);
            commonDialog.pop();
        }


        return;
    }
}

void DeleteAccountDialog::on_pwdLineEdit_returnPressed()
{
    on_okBtn_clicked();
}
