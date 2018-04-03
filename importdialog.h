// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ImportDialog;
}

class ShadowWidget;

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

    void pop();

signals:
    void accountImported();

private slots:
    void on_cancelBtn_clicked();

    void on_pathBtn_clicked();

    void on_importBtn_clicked();

    void jsonDataUpdated(QString id);

    void on_privateKeyLineEdit_textChanged(const QString &arg1);

    void on_privateKeyLineEdit_returnPressed();

private:
    Ui::ImportDialog *ui;
    ShadowWidget* shadowWidget;
    QString privateKey;

    bool accountNameAlreadyExisted(QString name);
};

#endif // IMPORTDIALOG_H
