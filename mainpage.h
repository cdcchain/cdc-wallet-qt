#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include <QMap>
#include <QTimer>

namespace Ui {
class MainPage;
}

#define MODULE_MAIN_PAGE "MAIN_PAGE"

class AccountDetailWidget;

class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = 0);
    ~MainPage();

    void updateAccountList();
    void retranslator(QString language);

    void getAssets();


public slots:
    void refresh();

    void init();

signals:
    void openAccountPage(QString);
    void showShadowWidget();
    void hideShadowWidget();
    void showApplyDelegatePage(QString);
    void refreshAccountInfo();
    void showTransferPage(QString);
    void newAccount(QString);

private slots:
    void importAccount();

    void addAccount();

    void on_addAccountBtn_clicked();

    void on_accountTableWidget_cellClicked(int row, int column);

    void on_accountTableWidget_cellEntered(int row, int column);

    void jsonDataUpdated(QString id);

    void showExportDialog(QString name);

    void withdrawSalary(QString name, QString salary);

    void renameAccount(QString name);

    void deleteAccount(QString name);

//    void showDetailWidget(QString name);

//    void hideDetailWidget();

    void on_scanBtn_clicked();
	
    void on_assetComboBox_currentIndexChanged(int index);

private:
    Ui::MainPage *ui;
    int previousColorRow;
    bool hasDelegateOrNot;
    bool refreshOrNot;
    AccountDetailWidget* detailWidget;
    int currentAccountIndex;
	QMovie* gif;
    bool assetUpdating;

//    bool eventFilter(QObject *watched, QEvent *e);
//    void paintOnScrollarea(QWidget *w);
    void paintEvent(QPaintEvent*);
    void updateTotalBalance();
    void updatePending();
    bool eventFilter(QObject *watched, QEvent *e);
    void showWarningDialog();

};

#endif // MAINPAGE_H
