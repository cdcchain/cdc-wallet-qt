#ifndef ASSETPAGE_H
#define ASSETPAGE_H

#include <QWidget>

namespace Ui {
class AssetPage;
}

class AssetPage : public QWidget
{
    Q_OBJECT

public:
    explicit AssetPage(QWidget *parent = 0);
    ~AssetPage();

    void updateAssetInfo();
    void updateMyAsset();

private slots:
    void jsonDataUpdated(QString id);

    void on_myAssetWidget_cellClicked(int row, int column);

    void on_addTokenBtn_clicked();

    void on_myAssetWidget_cellPressed(int row, int column);

private:
    Ui::AssetPage *ui;

    void paintEvent(QPaintEvent*);

};

#endif // ASSETPAGE_H
