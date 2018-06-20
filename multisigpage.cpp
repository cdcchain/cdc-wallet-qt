// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "multisigpage.h"
#include "ui_multisigpage.h"
#include "pubic_define.h"
#include "blockchain.h"
#include "rpcthread.h"

#include <QDebug>
#include <QPainter>

MultiSigPage::MultiSigPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiSigPage)
{
    ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(BACKGROUND_COLOR));
    setPalette(palette);

    connect( CDC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

}

MultiSigPage::~MultiSigPage()
{
    delete ui;
}

void MultiSigPage::on_toSignTextEdit_textChanged()
{
    ui->transactionInfoTextBrowser->clear();
    ui->afterSignTextBrowser->clear();

    QString text = ui->toSignTextEdit->toPlainText();
    if( text.simplified().isEmpty())
    {
        return;
    }

    parseToSignTransaction(text);
}

void MultiSigPage::parseToSignTransaction(QString text)
{

}
