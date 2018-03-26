#include "deleteaccountdialog.h"
#include "ui_deleteaccountdialog.h"
#include "blockchain.h"
#include "rpcthread.h"
#include "commondialog.h"

#include <QDebug>
#include <QFocusEvent>

DeleteAccountDialog::DeleteAccountDialog(QString name , QWidget *parent) :
    QDialog(parent),
    accountName(name),
    yesOrNo( false),
    ui(new Ui::DeleteAccountDialog)
{
    ui->setupUi(this);

//    Hcash::getInstance()->appendCurrentDialogVector(this);
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



    ui->okBtn->setText(tr("Ok"));

    ui->cancelBtn->setText(tr("Cancel"));

    ui->okBtn->setEnabled(false);

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


//    if( ui->pwdLineEdit->text().isEmpty()) return;

//    QString str = "wallet_check_passphrase " + ui->pwdLineEdit->text() + "\n";
//    Hcash::getInstance()->write( str);
//    QString result = Hcash::getInstance()->read();

//    if( result.mid(0,4) == "true")
//    {

//        Hcash::getInstance()->write( "wallet_account_delete " + accountName + "\n");
//        QString result = Hcash::getInstance()->read();

//        if( result.mid(0,4) == "true")
//        {
//            Hcash::getInstance()->deleteAccountInConfigFile( accountName);
//            Hcash::getInstance()->addressMapRemove( accountName);
//            Hcash::getInstance()->balanceMapRemove( accountName);
//            Hcash::getInstance()->registerMapRemove( accountName);

//        }

//        yesOrNo = true;
//        close();
////        emit accepted();
//    }
//    else if( result.mid(0,5) == "false")
//    {
//        ui->tipLabel1->setPixmap(QPixmap(":/pic/pic2/wrong.png"));
//        ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr("Wrong Password!") + "</font></body>" );
//    }
//    else if( result.mid(0,5) == "20015")
//    {
//        ui->tipLabel1->setPixmap(QPixmap(":/pic/pic2/wrong.png"));
//        ui->tipLabel2->setText("<body><font style=\"font-size:12px\" color=#FF8880>" + tr("At least 8 letters!") + "</font></body>" );
//    }

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

//    QEventLoop loop;
//    show();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

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
