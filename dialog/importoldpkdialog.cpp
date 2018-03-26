#include "importoldpkdialog.h"
#include "ui_importoldpkdialog.h"

#include <QDebug>

#include "blockchain.h"
#include "rpcthread.h"
#include "../commondialog.h"

ImportOldPkDialog::ImportOldPkDialog( QWidget *parent) :
    QDialog(parent),
    yesOrNO(false),
    ui(new Ui::ImportOldPkDialog)
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


    init();
}

ImportOldPkDialog::~ImportOldPkDialog()
{
    delete ui;
}

bool ImportOldPkDialog::pop()
{
    move(0,0);
    exec();

    return yesOrNO;
}

void ImportOldPkDialog::jsonDataUpdated(QString id)
{
    if( ui->decryptCheckBox->isChecked())
    {

    }
    else
    {

    }
}

void ImportOldPkDialog::on_okBtn_clicked()
{

}

void ImportOldPkDialog::on_cancelBtn_clicked()
{
    yesOrNO = false;
    close();
}

void ImportOldPkDialog::init()
{
    ui->decryptCheckBox->setChecked(false);
    ui->pwdLabel->hide();
    ui->pwdLineEdit->hide();
}

void ImportOldPkDialog::on_decryptCheckBox_stateChanged(int arg1)
{
    if(ui->decryptCheckBox->isChecked())
    {
        ui->pwdLabel->show();
        ui->pwdLineEdit->show();
    }
    else
    {
        ui->pwdLabel->hide();
        ui->pwdLineEdit->hide();
    }
}
