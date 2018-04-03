// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef SHADOWWIDGET_H
#define SHADOWWIDGET_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class ShadowWidget;
}

class ShadowWidget: public QWidget
{
    Q_OBJECT
public:
    ShadowWidget(QWidget *parent = 0);
    ~ShadowWidget();

    void init(QSize size);

    void retranslator();

private:
    Ui::ShadowWidget *ui;
    QMovie* gif;
};

#endif // SHADOWWIDGET_H
