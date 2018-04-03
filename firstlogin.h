// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef FIRSTLOGIN_H
#define FIRSTLOGIN_H

#include <QWidget>
#include <QLibrary>

namespace Ui {
class FirstLogin;
}

#define MODULE_FIRST_LOGIN "FIRST_LOGIN"

typedef short(*FUN)(int);

class FirstLogin : public QWidget
{
    Q_OBJECT

public:
    explicit FirstLogin(QWidget *parent = 0);
    ~FirstLogin();

private slots:
    void on_createBtn_clicked();

    void on_pwdLineEdit2_returnPressed();

    void on_minBtn_clicked();

    void on_closeBtn_clicked();

    void on_pwdLineEdit_textChanged(const QString &arg1);

    void on_pwdLineEdit2_textChanged(const QString &arg1);

    void jsonDataUpdated(QString id);

signals:
    void login();
    void minimum();
    void closeWallet();
    void tray();
    void showShadowWidget();
    void hideShadowWidget();

private:
    Ui::FirstLogin *ui;

    void paintEvent(QPaintEvent*);
    bool eventFilter(QObject *watched, QEvent *e);
    void keyPressEvent(QKeyEvent* e);
};

#endif // FIRSTLOGIN_H
