#include "commondialog.h"
#include "ui_commondialog.h"
#include <QDebug>
#include "debug_log.h"
#include "blockchain.h"

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
    ui->cancelBtn->setText(tr("Cancel"));

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
