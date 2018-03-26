#include "warningdialog.h"
#include "ui_warningdialog.h"

#include <QDebug>

#include "blockchain.h"
#include "rpcthread.h"
#include "../commondialog.h"


WarningDialog::WarningDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WarningDialog)
{
    ui->setupUi(this);

    setParent(CDC::getInstance()->mainFrame);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(255, 255, 255);border:1px groove rgb(180,180,180);}");

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimerOut()));
    timer->start(1000);

    sec = 10;

    ui->okBtn->setText(tr("I have known") + QString(" (%1)").arg(sec));
    ui->okBtn->setEnabled(false);


}

WarningDialog::~WarningDialog()
{
    delete ui;
}

void WarningDialog::pop()
{
    move(0,0);
    exec();
}

void WarningDialog::on_okBtn_clicked()
{
    close();
}

void WarningDialog::onTimerOut()
{
    sec--;
    if(sec > 0)
    {
        ui->okBtn->setText(tr("I have known") + QString(" (%1)").arg(sec));
    }
    else
    {
        timer->stop();
        ui->okBtn->setText(tr("I have known"));
        ui->okBtn->setEnabled(true);
    }
}
