// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "remarkdialog.h"
#include "ui_remarkdialog.h"
#include "blockchain.h"
#include <QTextStream>
#include <QDebug>
#include "debug_log.h"

RemarkDialog::RemarkDialog(QString address, QWidget *parent) :
    QDialog(parent),
    name(address),
    ui(new Ui::RemarkDialog)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    ui->setupUi(this);

    setParent(CDC::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(246, 246, 246);border:1px groove rgb(180,180,180);}");



    ui->okBtn->setText(tr("Ok"));
    ui->cancelBtn->setText(tr("Cancel"));

    ui->remarkLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->remarkLineEdit->setTextMargins(8,0,0,0);

    ui->remarkLineEdit->setFocus();
    DLOG_QT_WALLET_FUNCTION_END;
}

RemarkDialog::~RemarkDialog()
{
    delete ui;
}

void RemarkDialog::pop()
{
    move(0,0);
    exec();
}

void RemarkDialog::on_okBtn_clicked()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    if( !CDC::getInstance()->contactsFile->open(QIODevice::ReadWrite))
    {
        qDebug() << "contact.dat not exist";
        return;
    }

    QByteArray ba = QByteArray::fromBase64( CDC::getInstance()->contactsFile->readAll());
    ba += (name + '=' + ui->remarkLineEdit->text() + ";").toUtf8();
    ba = ba.toBase64();
    CDC::getInstance()->contactsFile->resize(0);
    QTextStream ts(CDC::getInstance()->contactsFile);
    ts << ba;

    CDC::getInstance()->contactsFile->close();

    close();

    DLOG_QT_WALLET_FUNCTION_END;
}

void RemarkDialog::on_cancelBtn_clicked()
{
    close();
}

void RemarkDialog::on_remarkLineEdit_returnPressed()
{
    on_okBtn_clicked();
}

void RemarkDialog::on_remarkLineEdit_textChanged(const QString &arg1)
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
