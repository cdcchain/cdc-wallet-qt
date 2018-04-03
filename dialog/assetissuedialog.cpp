// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "assetissuedialog.h"
#include "ui_assetissuedialog.h"

#include <QDebug>

#include "blockchain.h"
#include "rpcthread.h"
#include "../commondialog.h"


AssetIssueDialog::AssetIssueDialog( int assetId, QWidget *parent) :
    QDialog(parent),
    id(assetId),
    ui(new Ui::AssetIssueDialog)
{
    ui->setupUi(this);

    setParent(CDC::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(246, 246, 246);border:1px groove rgb(180,180,180);}");

    ui->amountLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->amountLineEdit->setTextMargins(8,0,0,0);
    QRegExp rx1("^([0]|[1-9][0-9]{0,16})(?:\\.\\d{1,5})?$|(^\\t?$)");
    QRegExpValidator *pReg1 = new QRegExpValidator(rx1, this);
    ui->amountLineEdit->setValidator(pReg1);
    ui->amountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->addressLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->addressLineEdit->setTextMargins(8,0,0,0);
    QRegExp regx("[a-zA-Z0-9\-\.\ \n]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->addressLineEdit->setValidator( validator );
    ui->addressLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->remarkLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->remarkLineEdit->setTextMargins(8,0,0,0);

    connect( CDC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    AssetInfo info = CDC::getInstance()->assetInfoMap.value(id);
    ui->symbolLabel->setText( info.symbol);
    ui->leftAmountLabel->setText( getBigNumberString(info.maxSupply - info.currentSupply, info.precision));

}

AssetIssueDialog::~AssetIssueDialog()
{
    delete ui;
}

void AssetIssueDialog::pop()
{
    move(0,0);
    exec();
}

void AssetIssueDialog::on_okBtn_clicked()
{
    if( ui->addressLineEdit->text().simplified().isEmpty())     return;
    if( ui->amountLineEdit->text().simplified().isEmpty())      return;

    CDC::getInstance()->postRPC( toJsonFormat( "id_wallet_uia_issue", "wallet_uia_issue", QStringList() << ui->amountLineEdit->text()
                                               << ui->symbolLabel->text() << ui->addressLineEdit->text() << ui->remarkLineEdit->text() ));

}

void AssetIssueDialog::on_cancelBtn_clicked()
{
    close();
}

void AssetIssueDialog::jsonDataUpdated(QString id)
{
    if( id == "id_wallet_uia_issue")
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            CDC::getInstance()->scanLater();

            ui->amountLineEdit->clear();

            int pos = result.indexOf("\"entry_id\":") + 12;
            QString txId = result.mid( pos, result.indexOf( "\"", pos ) - pos );
            CDC::getInstance()->postRPC( toJsonFormat( "id_wallet_scan_transaction", "wallet_scan_transaction", QStringList() << txId << "true" ));

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText(QString::fromLocal8Bit("分配成功!"));
            commonDialog.setText(tr("Token issued!"));
            commonDialog.pop();

        }

        return;
    }

    if( id == "id_wallet_scan_transaction")
    {
        QString result = CDC::getInstance()->jsonDataValue(id);
        qDebug() << result;

        return;
    }

}

void AssetIssueDialog::on_amountLineEdit_textChanged(const QString &arg1)
{

}
