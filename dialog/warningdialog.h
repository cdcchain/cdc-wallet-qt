#ifndef WARNINGDIALOG_H
#define WARNINGDIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class WarningDialog;
}

class WarningDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WarningDialog(QWidget *parent = 0);
    ~WarningDialog();

    void  pop();

private slots:
    void on_okBtn_clicked();

    void onTimerOut();

private:
    Ui::WarningDialog *ui;

    int sec;
    QTimer* timer;
};

#endif // WARNINGDIALOG_H
