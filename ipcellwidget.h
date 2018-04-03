// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef IPCELLWIDGET_H
#define IPCELLWIDGET_H

#include <QWidget>

namespace Ui {
class IpCellWidget;
}

class IpCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IpCellWidget( QString ip, bool flag, QWidget *parent = 0);
    ~IpCellWidget();

private slots:
    void on_nodeSetBtn_clicked();

signals:
    void blockSet(QString,bool);

private:
    Ui::IpCellWidget *ui;
    bool blackOrNot;
};

#endif // IPCELLWIDGET_H
