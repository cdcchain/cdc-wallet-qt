// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "showbottombarwidget.h"

ShowBottomBarWidget::ShowBottomBarWidget(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255,255,255));
    setPalette(palette);
}

ShowBottomBarWidget::~ShowBottomBarWidget()
{

}

void ShowBottomBarWidget::enterEvent(QEvent *e)
{
    emit showBottomBar();
}

