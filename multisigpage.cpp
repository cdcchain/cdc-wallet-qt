#include "multisigpage.h"
#include "ui_multisigpage.h"

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
    palette.setColor(QPalette::Background, QColor(244,244,242));
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
