// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef MULTISIGPAGE_H
#define MULTISIGPAGE_H

#include <QWidget>

namespace Ui {
class MultiSigPage;
}

class MultiSigPage : public QWidget
{
    Q_OBJECT

public:
    explicit MultiSigPage(QWidget *parent = 0);
    ~MultiSigPage();

private slots:
    void on_toSignTextEdit_textChanged();

private:
    Ui::MultiSigPage *ui;

    void parseToSignTransaction(QString text);
};

#endif // MULTISIGPAGE_H
