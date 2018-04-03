// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef SHOWBOTTOMBARWIDGET_H
#define SHOWBOTTOMBARWIDGET_H

#include <QWidget>

class ShowBottomBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowBottomBarWidget(QWidget *parent = 0);
    ~ShowBottomBarWidget();

protected:
    void enterEvent(QEvent* e);

signals:
    void showBottomBar();

};

#endif // SHOWBOTTOMBARWIDGET_H
