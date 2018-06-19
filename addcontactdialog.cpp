// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "addcontactdialog.h"
#include "ui_addcontactdialog.h"
#include "blockchain.h"
#include "rpcthread.h"
#include "pubic_define.h"
#include <QDebug>
#include <QMovie>
#include <QPainter>

AddContactDialog::AddContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddContactDialog)
{
    ui->setupUi(this);

    setParent(CDC::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(246, 246, 246);border:1px groove rgb(180,180,180);}");

    ui->containerWidget->installEventFilter(this);

    connect( CDC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    QRegExp regx("[a-zA-Z0-9\-\.\ \n]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->addressLineEdit->setValidator( validator );

    ui->addressLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->addressLineEdit->setTextMargins(8,0,0,0);
    ui->addressLineEdit->setPlaceholderText( tr("Please enter an account address."));
    ui->addressLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->remarkLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->remarkLineEdit->setTextMargins(8,0,0,0);


    ui->okBtn->setEnabled(false);
    ui->okBtn->setStyleSheet("QToolButton{background-color:rgb("STR_BUTTON_COLOR");color:#ffffff;border:none;border-radius:3px;}QToolButton:hover{background-color:rgb("STR_BUTTON_COLOR");}");
    ui->cancelBtn->setStyleSheet("QToolButton{background-color:#ffffff;color:#484848;border:1px solid rgb("STR_BUTTON_COLOR");border-radius:3px;}QToolButton:hover{color:rgb("STR_BUTTON_COLOR");}");

    ui->addressLineEdit->setFocus();

    gif = new QMovie(":/pic/pic2/loading2.gif");
    gif->setScaledSize( QSize(18,18));
    ui->gifLabel->setMovie(gif);
    gif->start();
    ui->gifLabel->hide();
}

AddContactDialog::~AddContactDialog()
{
    delete ui;
}

void AddContactDialog::pop()
{
    move(0,0);
    exec();
}

void AddContactDialog::on_cancelBtn_clicked()
{
    close();
//    emit accepted();
}

void AddContactDialog::jsonDataUpdated(QString id)
{
}

void AddContactDialog::on_okBtn_clicked()
{
    if( ui->addressLineEdit->text().simplified().isEmpty())
    {
        ui->tipLabel->setText(tr("can not be empty"));
        ui->tipLabel2->setPixmap(QPixmap(":/pic/pic2/wrong.png"));
        return;
    }

    if( !CDC::getInstance()->contactsFile->open(QIODevice::ReadWrite))
    {
        qDebug() << "contact.dat not exist";
        return;
    }


    QByteArray ba = QByteArray::fromBase64( CDC::getInstance()->contactsFile->readAll());
    QString str(ba);

    QStringList strList = str.split(";");
    strList.removeLast();
    foreach (QString ss, strList)
    {
        if( (ss.mid(0, ss.indexOf('=')) == ui->addressLineEdit->text())
          )
        {
            ui->tipLabel->setText(tr("Already existed"));
            ui->tipLabel2->setPixmap(QPixmap(":/pic/pic2/wrong.png"));
            CDC::getInstance()->contactsFile->close();
            return;
        }

        QString remark = (ss.mid(ss.indexOf('=') + 1));
        if(remark.isEmpty())    continue;
        if(  remark == ui->remarkLineEdit->text())
        {
            ui->tipLabel3->setText(tr("Already existed"));
            ui->tipLabel4->setPixmap(QPixmap(":/pic/pic2/wrong.png"));
            CDC::getInstance()->contactsFile->close();
            return;
        }
    }


    ba += ui->addressLineEdit->text().toUtf8() + '=' + ui->remarkLineEdit->text().toUtf8() + QString(";").toUtf8();
    ba = ba.toBase64();
    CDC::getInstance()->contactsFile->resize(0);
    QTextStream ts(CDC::getInstance()->contactsFile);
    ts << ba;

    CDC::getInstance()->contactsFile->close();

    close();
}

void AddContactDialog::on_remarkLineEdit_textChanged(const QString &arg1)
{
    QString remark = arg1;
    if( remark.contains("=") || remark.contains(";") || remark.contains(" ") )
    {
        remark.remove("=");
        remark.remove(";");
        remark.remove(" ");
        ui->remarkLineEdit->setText( remark);
    }

    ui->tipLabel3->setText("");
    ui->tipLabel4->setPixmap(QPixmap(""));
}


bool AddContactDialog::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->containerWidget)
    {
        if( e->type() == QEvent::Paint)
        {
            return true;
        }
    }

    return QWidget::eventFilter(watched,e);
}

void AddContactDialog::on_addressLineEdit_textEdited(const QString &arg1)
{
    QString address = arg1;
    address.remove(" ");
    address.remove("\n");
    ui->addressLineEdit->setText( address);
    if( ui->addressLineEdit->text().isEmpty())
    {
        ui->tipLabel->setText("");
        ui->tipLabel2->setPixmap(QPixmap(""));
        ui->okBtn->setEnabled(false);
        return;
    }

    if( checkAddress(ui->addressLineEdit->text()))
    {
        ui->tipLabel->setText("");
        ui->tipLabel2->setPixmap(QPixmap(""));
        ui->okBtn->setEnabled(true);
        return;
    }

    ui->tipLabel->setText(tr("Invalid address"));
    ui->tipLabel2->setPixmap(QPixmap(":/pic/pic2/wrong.png"));
    ui->okBtn->setEnabled(false);
}
