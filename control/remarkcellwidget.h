// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef REMARKCELLWIDGET_H
#define REMARKCELLWIDGET_H

#include <QWidget>

namespace Ui {
class RemarkCellWidget;
}

class RemarkCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RemarkCellWidget( QString str, QWidget *parent = 0);
    ~RemarkCellWidget();

    QString text();

private:
    Ui::RemarkCellWidget *ui;
    QString ss;
};

#endif // REMARKCELLWIDGET_H
