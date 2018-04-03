// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef COMMONTIP_H
#define COMMONTIP_H

#include <QDialog>

namespace Ui {
class CommonTip;
}

class CommonTip : public QDialog
{
    Q_OBJECT

public:
    explicit CommonTip(QWidget *parent = 0);
    ~CommonTip();

    void setText(QString);

private:
    Ui::CommonTip *ui;
};

#endif // COMMONTIP_H
