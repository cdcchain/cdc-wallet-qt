#ifndef IMPORTOLDPKDIALOG_H
#define IMPORTOLDPKDIALOG_H

#include <QDialog>

namespace Ui {
class ImportOldPkDialog;
}

class ImportOldPkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportOldPkDialog(QWidget *parent = 0);
    ~ImportOldPkDialog();

    bool  pop();

private slots:
    void jsonDataUpdated( QString id);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_decryptCheckBox_stateChanged(int arg1);

private:
    Ui::ImportOldPkDialog *ui;
    bool yesOrNO;

    void init();
};

#endif // IMPORTOLDPKDIALOG_H
