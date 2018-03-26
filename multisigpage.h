#ifndef MULTISIGPAGE_H
#define MULTISIGPAGE_H

#include <QWidget>

namespace Ui {
class MultiSigPage;
}

class MultiSigPage : public QWidget
{
    Q_OBJECT

public:
    explicit MultiSigPage(QWidget *parent = 0);
    ~MultiSigPage();

private slots:
    void on_toSignTextEdit_textChanged();

private:
    Ui::MultiSigPage *ui;

    void parseToSignTransaction(QString text);
};

#endif // MULTISIGPAGE_H
