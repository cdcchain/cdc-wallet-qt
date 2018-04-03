// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef SEARCHOPTIONWIDGET_H
#define SEARCHOPTIONWIDGET_H

#include <QWidget>

namespace Ui {
class SearchOptionWidget;
}

class SearchOptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchOptionWidget(QWidget *parent = 0);
    ~SearchOptionWidget();
    int currentChoice;

private slots:
    void on_radioButton1_clicked();

    void on_radioButton2_clicked();

    void on_radioButton3_clicked();

private:
    Ui::SearchOptionWidget *ui;

};

#endif // SEARCHOPTIONWIDGET_H
