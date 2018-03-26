#ifndef FUNCTIONBAR_H
#define FUNCTIONBAR_H

#include <QWidget>

namespace Ui {
class FunctionBar;
}

class FunctionBar : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionBar(QWidget *parent = 0);
    ~FunctionBar();
    void choosePage(int);
    void retranslator();

signals:
    void showMainPage();
    void showAssetPage();
    void showTransferPage();
    void showMultiSigPage();
//    void showMarketPage();
    void showContactPage();


private slots:
    void on_accountBtn_clicked();

    void on_assetBtn_clicked();

    void on_transferBtn_clicked();

    void on_multiSigBtn_clicked();

//    void on_marketBtn_clicked();

    void on_contactBtn_clicked();



private:
    Ui::FunctionBar *ui;
};

#endif // FUNCTIONBAR_H
