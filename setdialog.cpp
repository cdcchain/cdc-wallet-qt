// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <QPainter>
#include <QMouseEvent>
#include <QUrl>
#include <QDesktopServices>

#include "setdialog.h"
#include "ui_setdialog.h"
#include "blockchain.h"
#include "debug_log.h"
#include "rpcthread.h"
#include "commondialog.h"
#include "pubic_define.h"


#define SETDIALOG_GENERALBTN_SELECTED_STYLE     "QToolButton{background-color:rgb("STR_BUTTON_COLOR");color:white;border:1px solid rgb("STR_BUTTON_COLOR");border-top-left-radius: 3px;border-bottom-left-radius: 3px;}"
#define SETDIALOG_GENERALBTN_UNSELECTED_STYLE   "QToolButton{background-color:white;color:rgb("STR_BUTTON_COLOR");border:1px solid rgb("STR_BUTTON_COLOR");border-top-left-radius: 3px;border-bottom-left-radius: 3px;}"
#define SETDIALOG_SAVEBTN_SELECTED_STYLE        "QToolButton{background-color:rgb("STR_BUTTON_COLOR");color:white;border:1px solid rgb("STR_BUTTON_COLOR");border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
#define SETDIALOG_SAVEBTN_UNSELECTED_STYLE      "QToolButton{background-color:white;color:rgb("STR_BUTTON_COLOR");border:1px solid rgb("STR_BUTTON_COLOR");border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"


SetDialog::SetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetDialog)
{
    ui->setupUi(this);

    setParent(CDC::getInstance()->mainFrame);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet("#widget {background-color:rgba(10, 10, 10,100);}");
    ui->containerWidget->setObjectName("containerwidget");
    ui->containerWidget->setStyleSheet("#containerwidget{background-color: rgb(246, 246, 246);border:1px groove rgb(180,180,180);}");
    ui->titleLabel->setStyleSheet("background-color:rgb("STR_DIALOG_TITLE_COLOR");");
    ui->containerWidget->installEventFilter(this);
    ui->saveBtn->setStyleSheet("background-color:rgb("STR_BUTTON_COLOR");color:#ffffff;border:none;border-radius:3px;}");
    ui->confirmBtn->setStyleSheet("QToolButton{background-color:rgb("STR_BUTTON_COLOR");color:#ffffff;border:none;border-radius:3px;}QToolButton:disabled{background-color:#cecece;}");

    connect( CDC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    on_generalBtn_clicked();

    ui->lockTimeSpinBox->setValue(QString::number(CDC::getInstance()->lockMinutes).toInt());

    if(!CDC::getInstance()->notProduce)
    {
        ui->nolockCheckBox->setChecked(true);
        ui->lockTimeSpinBox->setEnabled(false);
    }
    else
    {
        ui->nolockCheckBox->setChecked(false);
        ui->lockTimeSpinBox->setEnabled(true);
    }

    QString language = CDC::getInstance()->language;
    if( language == "Simplified Chinese")
    {
        ui->languageComboBox->setCurrentIndex(0);
    }
    else if( language == "English")
    {
        ui->languageComboBox->setCurrentIndex(1);
    }
    else
    {
        ui->languageComboBox->setCurrentIndex(0);
    }

    ui->minimizeCheckBox->setChecked( CDC::getInstance()->minimizeToTray);
    ui->closeCheckBox->setChecked( CDC::getInstance()->closeToMinimize);
    ui->resyncCheckBox->setChecked( CDC::getInstance()->resyncNextTime);
    ui->feeLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->feeLineEdit->setTextMargins(8,0,0,0);
    ui->feeLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    qDebug() << "transactionFee: " << CDC::getInstance()->transactionFee;
    ui->feeLineEdit->setText(QString::number(float_t(CDC::getInstance()->transactionFee) / CDC::getInstance()->assetInfoMap.value(0).precision));

    QRegExp rx("^([1-9]\\d*)(?:\\.\\d{0,2})?$|(^\\t?$)");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->feeLineEdit->setValidator(pReg);

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/close4.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

#ifdef WIN32
    ui->languageComboBox->setStyleSheet("QComboBox {border: 1px solid gray;border-radius: 3px;padding: 1px 2px 1px 8px;min-width: 5em;}"
                                    "QComboBox::drop-down {subcontrol-origin: padding;subcontrol-position: top right;width: 20px;"
                                                           "border-left-width: 1px;border-left-color: darkgray;border-left-style: solid;"
                                                           "border-top-right-radius: 3px;border-bottom-right-radius: 3px;}"
                                    "QComboBox::down-arrow {image: url(:/pic/pic2/comboBoxArrow.png);}"
                                    );
#endif

    ui->confirmBtn->setEnabled(false);

    ui->oldPwdLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->oldPwdLineEdit->setTextMargins(8,0,0,0);
    ui->oldPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->newPwdLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->newPwdLineEdit->setTextMargins(8,0,0,0);
    ui->newPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->confirmPwdLineEdit->setStyleSheet("color:black;border:1px solid #CCCCCC;border-radius:3px;");
    ui->confirmPwdLineEdit->setTextMargins(8,0,0,0);
    ui->confirmPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->oldPwdLineEdit->setMaxLength(14);
    ui->newPwdLineEdit->setMaxLength(14);
    ui->confirmPwdLineEdit->setMaxLength(14);

    ui->oldPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->newPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->confirmPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->lockTimeSpinBox->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->minimizeCheckBox->setStyleSheet("QCheckBox::indicator{ image:url(:/pic/pic2/checkBox_unchecked.png); }"
                                    "QCheckBox::indicator:checked{ image:url(:/pic/cplpic/checkBox_checked.png); }");
    ui->closeCheckBox->setStyleSheet("QCheckBox::indicator{ image:url(:/pic/pic2/checkBox_unchecked.png); }"
                                    "QCheckBox::indicator:checked{ image:url(:/pic/cplpic/checkBox_checked.png); }");
    ui->nolockCheckBox->setStyleSheet("QCheckBox::indicator{ image:url(:/pic/pic2/checkBox_unchecked.png); }"
                                    "QCheckBox::indicator:checked{ image:url(:/pic/cplpic/checkBox_checked.png); }");
    ui->resyncCheckBox->setStyleSheet("QCheckBox::indicator{ image:url(:/pic/pic2/checkBox_unchecked.png); }"
                                    "QCheckBox::indicator:checked{ image:url(:/pic/cplpic/checkBox_checked.png); }");



}

SetDialog::~SetDialog()
{
    qDebug() << "setdialog delete";
    delete ui;
}

void SetDialog::pop()
{
    move(0,0);
    exec();
}


bool SetDialog::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->containerWidget)
    {
        if( e->type() == QEvent::Paint)
        {
            QPainter painter(ui->containerWidget);
            painter.setPen(QPen(QColor(122,112,110),Qt::SolidLine));
            painter.setBrush(QBrush(QColor(122,112,110),Qt::SolidPattern));
            painter.drawRect(0,0,628,50);

            return true;
        }
    }

    return QWidget::eventFilter(watched,e);
}

void SetDialog::on_closeBtn_clicked()
{
    close();
}

void SetDialog::on_saveBtn_clicked()
{
    mutexForConfigFile.lock();
    CDC::getInstance()->lockMinutes = ui->lockTimeSpinBox->value();
    CDC::getInstance()->configFile->setValue("/settings/lockMinutes", CDC::getInstance()->lockMinutes);
    if( ui->nolockCheckBox->isChecked())
    {
        CDC::getInstance()->notProduce = false;
    }
    else
    {
        CDC::getInstance()->notProduce = true;
    }
    CDC::getInstance()->configFile->setValue("/settings/notAutoLock", !CDC::getInstance()->notProduce);

    if( ui->languageComboBox->currentIndex() == 0)      // config 中保存语言设置
    {
        CDC::getInstance()->configFile->setValue("/settings/language", "Simplified Chinese");
        CDC::getInstance()->language = "Simplified Chinese";
    }
    else if( ui->languageComboBox->currentIndex() == 1)
    {
        CDC::getInstance()->configFile->setValue("/settings/language", "English");
        CDC::getInstance()->language = "English";
    }


    CDC::getInstance()->minimizeToTray = ui->minimizeCheckBox->isChecked();
    CDC::getInstance()->configFile->setValue("/settings/minimizeToTray", CDC::getInstance()->minimizeToTray);

    CDC::getInstance()->closeToMinimize = ui->closeCheckBox->isChecked();
    CDC::getInstance()->configFile->setValue("/settings/closeToMinimize", CDC::getInstance()->closeToMinimize);

    CDC::getInstance()->resyncNextTime = ui->resyncCheckBox->isChecked();
    CDC::getInstance()->configFile->setValue("/settings/resyncNextTime", CDC::getInstance()->resyncNextTime);


    mutexForConfigFile.unlock();

    qDebug() << "fee: " << QString::number(ui->feeLineEdit->text().toFloat() * CDC::getInstance()->assetInfoMap.value(0).precision);
    CDC::getInstance()->postRPC(
                toJsonFormat(
                    "id_wallet_set_transaction_fee",
                    "wallet_set_transaction_fee",
                    QStringList() << ui->feeLineEdit->text()));

    emit settingSaved();

}

void SetDialog::on_nolockCheckBox_clicked()
{
    if(ui->nolockCheckBox->isChecked())
    {
        ui->lockTimeSpinBox->setEnabled(false);
    }
    else
    {
        ui->lockTimeSpinBox->setEnabled(true);
    }
}

void SetDialog::on_lockTimeSpinBox_valueChanged(const QString &arg1)
{
    if( arg1.startsWith('0') || arg1.isEmpty())
    {
        ui->lockTimeSpinBox->setValue(1);
    }
}

void SetDialog::on_generalBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->generalBtn->setStyleSheet(SETDIALOG_GENERALBTN_SELECTED_STYLE);
    ui->safeBtn->setStyleSheet(SETDIALOG_SAVEBTN_UNSELECTED_STYLE);
}

void SetDialog::on_safeBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->generalBtn->setStyleSheet(SETDIALOG_GENERALBTN_UNSELECTED_STYLE);
    ui->safeBtn->setStyleSheet(SETDIALOG_SAVEBTN_SELECTED_STYLE);
}

void SetDialog::on_confirmBtn_clicked()
{
    ui->tipLabel3->clear();

    CDC::getInstance()->postRPC( toJsonFormat( "id_wallet_check_passphrase", "wallet_check_passphrase", QStringList() << ui->oldPwdLineEdit->text() ));

}

void SetDialog::on_newPwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->newPwdLineEdit->setText( ui->newPwdLineEdit->text().remove(' '));
        return;
    }

    if( arg1.isEmpty())
    {
        ui->tipLabel->setText("");
        ui->confirmBtn->setEnabled(false);

        return;
    }

    if( ui->newPwdLineEdit->text().length() < 8)
    {
        ui->tipLabel->setText(tr("at least 8 letters"));
        ui->confirmBtn->setEnabled(false);

        return;
    }
    else if( ui->newPwdLineEdit->text() == ui->oldPwdLineEdit->text())
    {
        ui->tipLabel->setText(tr("same password"));
        ui->confirmBtn->setEnabled(false);

        return;
    }
    else
    {
        ui->tipLabel->clear();
    }

    if( ui->confirmPwdLineEdit->text().isEmpty())
    {
        ui->tipLabel2->clear();
        ui->confirmBtn->setEnabled(false);

        return;
    }

    if( ui->newPwdLineEdit->text() == ui->confirmPwdLineEdit->text())
    {
        ui->tipLabel2->clear();
        ui->confirmBtn->setEnabled(true);

    }
    else
    {
        ui->tipLabel2->setText(tr("not consistent"));
        ui->confirmBtn->setEnabled(false);

    }
}

void SetDialog::on_confirmPwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->confirmPwdLineEdit->setText( ui->confirmPwdLineEdit->text().remove(' '));
        return;
    }

    if( arg1.isEmpty())
    {
        ui->tipLabel2->setText("");
        ui->confirmBtn->setEnabled(false);

        return;
    }

    if( ui->confirmPwdLineEdit->text().length() < 8)
    {
        ui->tipLabel2->setText(tr("at least 8 letters"));
        ui->confirmBtn->setEnabled(false);

        return;
    }
    else
    {
        ui->tipLabel2->clear();
    }


    if( ui->newPwdLineEdit->text() == ui->confirmPwdLineEdit->text())
    {

        if( ui->newPwdLineEdit->text() == ui->oldPwdLineEdit->text())
        {
            ui->confirmBtn->setEnabled(false);
        }
        else
        {
            ui->confirmBtn->setEnabled(true);
        }
    }
    else
    {
        ui->tipLabel2->setText(tr("not consistent"));
        ui->confirmBtn->setEnabled(false);
    }
}


void SetDialog::on_oldPwdLineEdit_textChanged(const QString &arg1)
{
    ui->tipLabel3->clear();

    if( arg1.indexOf(' ') > -1)
    {
        ui->oldPwdLineEdit->setText( ui->oldPwdLineEdit->text().remove( ' '));
    }

}

void SetDialog::jsonDataUpdated(QString id)
{
    if( id == "id_wallet_check_passphrase")
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        if( result == "\"result\":true")
        {
            CDC::getInstance()->postRPC( toJsonFormat( "id_wallet_change_passphrase", "wallet_change_passphrase", QStringList() << ui->oldPwdLineEdit->text() <<  ui->newPwdLineEdit->text() ));
        }
        else
        {
            ui->tipLabel3->setText( tr("wrong password"));
        }

        return;
    }

    if( id == "id_wallet_change_passphrase")
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        if( result == "\"result\":null")
        {
            close();
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            CommonDialog commonDialog(CommonDialog::OkOnly);
            commonDialog.setText( "Failed: " + errorMessage);
            commonDialog.pop();
        }

        return;
    }

    if( id == "id_wallet_set_transaction_fee")
    {
        QString result = CDC::getInstance()->jsonDataValue(id);

        if( result.startsWith("\"result\":"))
        {
            CDC::getInstance()->postRPC( toJsonFormat( "id_wallet_get_transaction_fee", "wallet_get_transaction_fee", QStringList() << "" ));
            close();
        }
        else
        {
            int pos = result.indexOf("\"message\":\"") + 11;
            QString errorMessage = result.mid(pos, result.indexOf("\"", pos) - pos);

            if( errorMessage == "invalid fee")
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( tr("The fee should not be 0!"));
                commonDialog.pop();
            }
            else
            {
                CommonDialog commonDialog(CommonDialog::OkOnly);
                commonDialog.setText( "Failed: " + errorMessage);
                commonDialog.pop();
            }
        }

        return;
    }
}

void SetDialog::on_languageComboBox_currentIndexChanged(const QString &arg1)
{

}
