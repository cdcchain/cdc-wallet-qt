// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef WORKERTHREADMANAGER_H
#define WORKERTHREADMANAGER_H

#include "workerthread.h"

#define NUM_OF_WORKERTHREADS  8

class WorkerThreadManager:public QObject
{
    Q_OBJECT
public:
    WorkerThreadManager();
    ~WorkerThreadManager();

public slots:

    void processRPCs( QString cmd);

private:
    QMap<int, WorkerThread*> workerThreads;
    bool running;
};

#endif // WORKERTHREADMANAGER_H
