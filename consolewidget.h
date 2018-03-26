#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QWidget>

namespace Ui {
class ConsoleWidget;
}

class ConsoleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConsoleWidget(QWidget *parent = 0);
    ~ConsoleWidget();

protected:
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_closeBtn_clicked();

    void on_consoleLineEdit_returnPressed();

    void jsonDataUpdated(QString id);

    void on_clearBtn_clicked();

private:
    Ui::ConsoleWidget *ui;
    QVector<QString> cmdVector;
    int cmdIndex;

    void paintEvent(QPaintEvent*);
    bool eventFilter(QObject *watched, QEvent *e);
    bool mouse_press;
    QPoint move_point;
};

#endif // CONSOLEWIDGET_H
