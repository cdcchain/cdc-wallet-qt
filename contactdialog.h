// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog>

namespace Ui {
class ContactDialog;
}

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactDialog(QWidget *parent = 0);
    ~ContactDialog();

    void updateContactsList();

private slots:

    void on_contactsTableWidget_cellClicked(int row, int column);

    void on_contactsTableWidget_cellEntered(int row, int column);

signals:
    void contactSelected(QString,QString);

private:
    Ui::ContactDialog *ui;
    int previousColorRow;
};

#endif // CONTACTDIALOG_H
