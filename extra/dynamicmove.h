// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef DYNAMICMOVE_H
#define DYNAMICMOVE_H

#include <QWidget>
#include <QTimer>
#include <QTime>

class DynamicMove : public QObject
{
    Q_OBJECT
public:
    DynamicMove( QWidget* widget, QPoint destinationPosition, int intervalSecs, int numOfFrames, QWidget* parent = 0);
    ~DynamicMove();

    void start();

private slots:
    void step();

signals:
    void moveEnd();

private:
    QWidget* w;
    QTimer timer;
    int intervalTime;
    int number;  // 总帧数
    QPoint initialPosition;
    QPoint destination;
    int count;
};

#endif // DYNAMICMOVE_H
