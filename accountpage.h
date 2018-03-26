#ifndef ACCOUNTPAGE_H
#define ACCOUNTPAGE_H

#include <QWidget>
#include "blockchain.h"

namespace Ui {
class AccountPage;
}

#define MODULE_ACCOUNT_PAGE "ACCOUNT_PAGE"


class AccountPage : public QWidget
{
    Q_OBJECT

public:
    explicit AccountPage(QString name,QWidget *parent = 0);
    ~AccountPage();

    void updateTransactionsList();

    void updateBalance();

    void updateAssetList();

public slots:
    void refresh();

private slots:

    void on_copyBtn_clicked();

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void jsonDataUpdated(QString id);

//    void on_accountTransactionsTableWidget_cellClicked(int row, int column);

    void on_prePageBtn_clicked();

    void on_nextPageBtn_clicked();

    void on_pageLineEdit_editingFinished();

    void on_pageLineEdit_textEdited(const QString &arg1);

    void on_assetComboBox_currentIndexChanged(int index);

    void on_accountTransactionsTableWidget_cellPressed(int row, int column);

    void on_qrcodeBtn_clicked();

signals:
    void back();
    void accountChanged(QString);
    void showShadowWidget();
    void hideShadowWidget();
    void showApplyDelegatePage(QString);

private:
    Ui::AccountPage *ui;
    QString accountName;
    QString address;
    int transactionType;
    bool inited;
    int currentPageIndex;
    QString registeredLabelString;
    bool assetUpdating;

    void paintEvent(QPaintEvent*e);
    void init();
    void showTransactions();
    void showNormalTransactions();
    void showContractTransactions();


    struct TransactionDetail
    {
        QString opposite;
        int type = 0;   // 0:默认/自己转自己  1:转出 2:转入    3:POW挖矿奖励  4:POS挖矿奖励  5:市场成交   6:市场撤单   7:调用合约
        AssetAmount assetAmount;
        QString memo;
        bool bothMyAccount = false; // 是否from和to都是自己的账户  (如果都是 ledger_entries会有两项)
    };
    TransactionDetail getDetail(TransactionInfo info);    // 从transactioninfo中获取交易对方   或者市场挂单/市场撤单/市场成交

};

#endif // ACCOUNTPAGE_H
