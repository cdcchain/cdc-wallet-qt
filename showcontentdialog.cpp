// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "showcontentdialog.h"
#include "ui_showcontentdialog.h"
#include <QClipboard>

ShowContentDialog::ShowContentDialog(QString text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowContentDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);


    setStyleSheet("#ShowContentDialog{background-color:rgb(238,237,238);border:1px groove rgb(215,215,215);}");
    ui->textLabel->setStyleSheet("background-color:rgb(238,237,238);");
    ui->textLabel->setText(text);
    ui->textLabel->adjustSize();

    ui->copyBtn->setStyleSheet("QToolButton{background-image:url(:/pic/cplpic/copy.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");
    ui->copyBtn->setToolTip(tr("copy to clipboard"));
    ui->copyBtn->move( 7, 1);
    ui->textLabel->move( 25, 1);

    setGeometry(0,0, ui->textLabel->width() + 30,20);
}

ShowContentDialog::~ShowContentDialog()
{
    delete ui;
}

void ShowContentDialog::on_copyBtn_clicked()
{
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText( ui->textLabel->text() );
    close();
}
