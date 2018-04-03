// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "blockchain.h"
#include "debug_log.h"
#include "workerthreadmanager.h"
#include "rpcthread.h"
#include <QTextCodec>
#include <QDebug>
#include <QObject>
#ifdef WIN32
#include <Windows.h>
#endif
#include <QTimer>
#include <QThread>
#include <QApplication>
#include <QFrame>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

CDC *CDC::goo = 0;
//QMutex mutexForJsonData;
//QMutex mutexForPending;
//QMutex mutexForConfigFile;
//QMutex mutexForMainpage;
//QMutex mutexForPendingFile;
//QMutex mutexForDelegateList;
//QMutex mutexForRegisterMap;
//QMutex mutexForBalanceMap;
//QMutex mutexForAddressMap;
//QMutex mutexForRpcReceiveOrNot;

CDC::CDC()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    proc = new QProcess;

    workerManager = NULL;

    getSystemEnvironmentPath();
    changeToWalletConfigPath();

    threadForWorkerManager = NULL;
    currentDialog = NULL;
    hasDelegateSalary = false;
    needToScan = false;
    currentPort = RPC_PORT;
    currentAccount = "";
    currentTokenAddress = "";
    transactionFee = 0;

    qDebug() << "wallet config path: " << walletConfigPath;
    configFile = new QSettings( walletConfigPath + "/config.ini", QSettings::IniFormat);
    if( configFile->value("/settings/lockMinutes").toInt() == 0)   // 如果第一次，没有config.ini
    {
        configFile->setValue("/settings/lockMinutes",5);
        lockMinutes     = 5;
        configFile->setValue("/settings/notAutoLock",false);
        notProduce      =  true;
        configFile->setValue("/settings/language","Simplified Chinese");
        language = "Simplified Chinese";
        minimizeToTray  = false;
        configFile->setValue("/settings/minimizeToTray",false);
        closeToMinimize = false;
        configFile->setValue("/settings/closeToMinimize",false);
        resyncNextTime = false;
        configFile->setValue("settings/resyncNextTime",false);
        firstUse = true;
        configFile->setValue("settings/firstUse",true);
    }
    else
    {
        lockMinutes     = configFile->value("/settings/lockMinutes").toInt();
        notProduce      = !configFile->value("/settings/notAutoLock").toBool();
        minimizeToTray  = configFile->value("/settings/minimizeToTray").toBool();
        closeToMinimize = configFile->value("/settings/closeToMinimize").toBool();
        language        = configFile->value("/settings/language").toString();
        resyncNextTime  = configFile->value("/settings/resyncNextTime",false).toBool();
        firstUse        = configFile->value("/settings/firstUse",true).toBool();
    }

    QFile file( walletConfigPath + "/log.txt");       // 每次启动清空 log.txt文件
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    file.close();

//    contactsFile = new QFile( "contacts.dat");
    contactsFile = NULL;

    pendingFile  = new QFile( walletConfigPath + "/pending.dat");

    consoleWidget = NULL;

    stopping = false;

    DLOG_QT_WALLET_FUNCTION_END;
}

CDC::~CDC()
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;

	if (configFile)
	{
		delete configFile;
		configFile = NULL;
	}

    if( contactsFile)
    {
        delete contactsFile;
        contactsFile = NULL;
    }

    if( threadForWorkerManager)
    {
        delete threadForWorkerManager;
        threadForWorkerManager = NULL;
    }

    if( workerManager)
    {
        delete workerManager;
        workerManager = NULL;
    }

    DLOG_QT_WALLET_FUNCTION_END;
}

CDC*   CDC::getInstance()
{
    if( goo == NULL)
    {
        goo = new CDC;
    }
    return goo;
}


qint64 CDC::write(QString cmd)
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QByteArray cmdBa = gbkCodec->fromUnicode(cmd);  // 转为gbk的bytearray
    proc->readAll();
    return proc->write(cmdBa.data());
}

QString CDC::read()
{
    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QString result;
    QString str;
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    while ( !result.contains(">>>"))        // 确保读取全部输出
    {
        proc->waitForReadyRead(50);
        str = gbkCodec->toUnicode(proc->readAll());
        result += str;
        if( str.right(2) == ": " )  break;

        //  手动调用处理未处理的事件，防止界面阻塞
//        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    QApplication::restoreOverrideCursor();
    return result;

}

void CDC::deleteAccountInConfigFile(QString accountName)
{
    DLOG_QT_WALLET_FUNCTION_BEGIN;
    mutexForConfigFile.lock();
    configFile->beginGroup("/accountInfo");
    QStringList keys = configFile->childKeys();

    int i = 0;
    for( ; i < keys.size(); i++)
    {
        if( configFile->value( keys.at(i)) == accountName)
        {
            break;
        }
    }

    for( ; i < keys.size() - 1; i++)
    {
        configFile->setValue( keys.at(i) , configFile->value( keys.at(i + 1)));
    }

    configFile->remove( keys.at(i));

    configFile->endGroup();
    mutexForConfigFile.unlock();

    DLOG_QT_WALLET_FUNCTION_END;
}


TwoAddresses CDC::getAddress(QString name)
{
    TwoAddresses twoAddresses;

    if( name.isEmpty())
    {
        return twoAddresses;
    }

    QString result = jsonDataValue("id_wallet_list_my_addresses");

    int pos = result.indexOf( "\"name\":\"" + name + "\",") ;
    if( pos != -1)  // 如果 wallet_list_my_addresses 中存在
    {

        int pos2 = result.indexOf( "\"owner_address\":", pos) + 17;
        twoAddresses.ownerAddress = result.mid( pos2, result.indexOf( "\"", pos2) - pos2);

        pos2 = result.indexOf( "\"active_address\":", pos) + 18;
        twoAddresses.activeAddress = result.mid( pos2, result.indexOf( "\"", pos2) - pos2);
    }

    return twoAddresses;
}

bool CDC::isMyAddress(QString address)
{
    if( address.isEmpty())
    {
        return false;
    }

    bool result = false;
    foreach (QString key, addressMap.keys())
    {
         if( addressMap.value(key).ownerAddress == address || addressMap.value(key).activeAddress == address)
         {
             result = true;
             break;
         }
    }

    return result;
}

QString CDC::addressToName(QString address)
{
    foreach (QString key, addressMap.keys())
    {
        if( addressMap.value(key).ownerAddress == address)
        {
            return key;
        }
    }

    return address;
}

QString CDC::getBalance(QString name)
{
    if( name.isEmpty())
    {
        return NULL;
    }


    QString result = jsonDataValue("id_balance");

    int p = result.indexOf( "[\"" + name + "\",");
    if( p != -1)  // 如果balance中存在
    {
        int pos = p + 8 + name.size();
        QString str = result.mid( pos, result.indexOf( "]", pos) - pos);
        str.remove("\"");

        double amount = str.toDouble() / CDC::getInstance()->assetInfoMap.value(0).precision;

        return doubleTo5Decimals( amount) + " " + QString(ASSET_NAME);
    }

    // balance中不存在
    return "0.00000 " + QString(ASSET_NAME);
}

QString CDC::getRegisterTime(QString name)
{
    if( name.isEmpty())
    {
        return NULL;
    }

    QString result = jsonDataValue("id_wallet_list_my_addresses");
    int pos = result.indexOf( "\"name\":\"" + name + "\",") ;
    if( pos != -1)  // 如果 wallet_list_my_addresses 中存在
    {
        int pos2 = result.indexOf( "\"registration_date\":", pos) + 21;
        QString registerTime = result.mid( pos2, result.indexOf( "\"", pos2) - pos2);

        if( registerTime != "1970-01-01T00:00:00")
        {
            return registerTime;
        }
        else
        {
            return "NO";
        }
    }

    return "WRONGACCOUNTNAME";
}

void CDC::getSystemEnvironmentPath()
{
    QStringList environment = QProcess::systemEnvironment();
    QString str;

#ifdef WIN32
    foreach(str,environment)
    {
        if (str.startsWith("APPDATA="))
        {
            walletConfigPath = str.mid(8) + "\\" + ASSET_NAME + "Wallet";
            appDataPath = walletConfigPath + "\\chaindata";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#elif defined(TARGET_OS_MAC)
    foreach(str,environment)
    {
        if (str.startsWith("HOME="))
        {
            walletConfigPath = str.mid(5) + "/Library/Application Support/CDCWallet";
            appDataPath = walletConfigPath + "/chaindata";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#else
    foreach(str,environment)
    {
        if (str.startsWith("HOME="))
        {
            appDataPath = str.mid(5) + "/"ASSET_NAME;
            walletConfigPath = str.mid(5) + "/CDCWallet";
            qDebug() << "appDataPath:" << appDataPath;
            break;
        }
    }
#endif
}

void CDC::changeToWalletConfigPath()
{
    QFile file("config.ini");
    if( !file.exists())     return;
    QFile file2(walletConfigPath + "/config.ini");
    qDebug() << file2.exists() << walletConfigPath + "/config.ini";
    if( file2.exists())
    {
        qDebug() << "remove config.ini : " << file.remove();
        return;
    }

    qDebug() << "copy config.ini : " << file.copy(walletConfigPath + "/config.ini");
    qDebug() << "remove old config.ini : " << file.remove();
}

void CDC::getContactsFile()
{
    QString path;
    if( configFile->contains("/settings/chainPath"))
    {
        path = configFile->value("/settings/chainPath").toString();
    }
    else
    {
        path = appDataPath;
    }

    contactsFile = new QFile(path + "/contacts.dat");
    if( contactsFile->exists())
    {
        // 如果数据路径下 有contacts.dat 则使用数据路径下的
        return;
    }
    else
    {
        QFile file2("contacts.dat");
        if( file2.exists())
        {
            // 如果数据路径下没有 钱包路径下有 将钱包路径下的剪切到数据路径下
       qDebug() << "contacts.dat copy" << file2.copy(path + "/contacts.dat");
       qDebug() << "contacts.dat copy" << file2.remove();
            return;
        }
        else
        {
            // 如果都没有 则使用钱包路径下的
        }
    }
}

void CDC::parseBalance()
{
    accountBalanceMap.clear();

    QString jsonResult = jsonDataValue("id_balance");
    jsonResult.prepend("{");
    jsonResult.append("}");

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonResult.toLatin1(), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if( parse_doucment.isObject())
        {
            QJsonObject jsonObject = parse_doucment.object();
            if( jsonObject.contains("result"))
            {
                QJsonValue resultValue = jsonObject.take("result");
                if( resultValue.isArray())
                {
                    QJsonArray resultArray = resultValue.toArray();
                    for( int i = 0; i < resultArray.size(); i++)
                    {
                        QJsonValue resultArrayValue = resultArray.at(i);
                        if( resultArrayValue.isArray())
                        {
                            QJsonArray array = resultArrayValue.toArray();
                            QJsonValue value = array.at(0);
                            QString account = value.toString();
                            AssetBalanceMap assetBalanceMap;

                            QJsonArray array2 = array.at(1).toArray();

                            for( int i = 0; i < array2.size(); i++)
                            {
                                QJsonArray array3 = array2.at(i).toArray();
                                int asset = array3.at(0).toInt();

                                unsigned long long balance;
                                if( array3.at(1).isString())
                                {
                                    balance = array3.at(1).toString().toULongLong();
                                }
                                else
                                {
                                    balance = QString::number(array3.at(1).toDouble(),'g',10).toULongLong();
                                }

                                assetBalanceMap.insert(asset, balance);

                            }
                            accountBalanceMap.insert(account,assetBalanceMap);
                        }
                    }
                }

            }
        }
    }


}


void CDC::parseAssetInfo()
{
    assetInfoMap.clear();

    QString jsonResult = jsonDataValue("id_blockchain_list_assets");
    jsonResult.prepend("{");
    jsonResult.append("}");

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(jsonResult.toLatin1(), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if( parse_doucment.isObject())
        {
            QJsonObject jsonObject = parse_doucment.object();
            if( jsonObject.contains("result"))
            {
                QJsonValue resultValue = jsonObject.take("result");
                if( resultValue.isArray())
                {
                    QJsonArray resultArray = resultValue.toArray();
                    for( int i = 0; i < resultArray.size(); i++)
                    {
                        AssetInfo assetInfo;

                        QJsonObject object = resultArray.at(i).toObject();
                        assetInfo.id = object.take("id").toInt();
                        assetInfo.symbol = object.take("symbol").toString();

                        QJsonObject object2 = object.take("authority").toObject();
                        assetInfo.owner = object2.take("owners").toArray().at(0).toString();

                        assetInfo.name = object.take("name").toString();
                        assetInfo.description = object.take("description").toString();

                        QJsonValue value = object.take("precision");
                        if( value.isString())
                        {
                            assetInfo.precision = value.toString().toULongLong();
                        }
                        else
                        {
                            assetInfo.precision = QString::number(value.toDouble(),'g',10).toULongLong();
                        }

                        QJsonValue value2 = object.take("max_supply");
                        if( value2.isString())
                        {
                            assetInfo.maxSupply = value2.toString().toULongLong();
                        }
                        else
                        {
                            assetInfo.maxSupply = QString::number(value2.toDouble(),'g',10).toULongLong();
                        }

                        QJsonValue value3 = object.take("current_supply");
                        if( value3.isString())
                        {
                            assetInfo.currentSupply = value3.toString().toULongLong();
                        }
                        else
                        {
                            assetInfo.currentSupply = QString::number(value3.toDouble(),'g',10).toULongLong();
                        }

                        assetInfo.registrationDate = object.take("registration_date").toString();

                        assetInfoMap.insert(assetInfo.id,assetInfo);

                    }
                }
            }
        }
    }

}

int CDC::getAssetId(QString symbol)
{
    int id = -1;
    foreach (int key, CDC::getInstance()->assetInfoMap.keys())
    {
        AssetInfo info = CDC::getInstance()->assetInfoMap.value(key);
        if( info.symbol == symbol)
        {
            id = key;
            continue;
        }
    }

    return id;
}

void CDC::parseTransactions(QString result, QString accountName)
{
    transactionsMap.remove(accountName);

    result.prepend("{");
    result.append("}");

    QTextCodec* utfCodec = QTextCodec::codecForName("UTF-8");
    QByteArray ba = utfCodec->fromUnicode(result);

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(ba, &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if( parse_doucment.isObject())
        {
            QJsonObject jsonObject = parse_doucment.object();
            if( jsonObject.contains("result"))
            {
                QJsonValue resultValue = jsonObject.take("result");
                if( resultValue.isArray())
                {
                    TransactionsInfoVector transactionsInfoVector;
                    QJsonArray resultArray = resultValue.toArray();
                    for( int i = 0; i < resultArray.size(); i++)
                    {
                        TransactionInfo transactionInfo;

                        QJsonObject object          = resultArray.at(i).toObject();
                        transactionInfo.isConfirmed = object.take("is_confirmed").toBool();
                        if ( !transactionInfo.isConfirmed)
                        {
                            // 包含error  则为失效交易
                            if( object.contains("error"))   continue;
                        }

                        transactionInfo.trxId       = object.take("trx_id").toString();
                        transactionInfo.isMarket    = object.take("is_market").toBool();
                        transactionInfo.isMarketCancel = object.take("is_market_cancel").toBool();
                        transactionInfo.blockNum    = object.take("block_num").toInt();
                        transactionInfo.timeStamp   = object.take("timestamp").toString();

                        QJsonArray entriesArray       = object.take("ledger_entries").toArray();
                        for( int j = 0; j < entriesArray.size(); j++)
                        {
                            QJsonObject entryObject = entriesArray.at(j).toObject();
                            Entry   entry;
                            entry.fromAccount       = entryObject.take("from_account").toString();
                            entry.toAccount         = entryObject.take("to_account").toString();
                            QJsonValue v = entryObject.take("memo");
                            entry.memo              = v.toString();

                            QJsonObject amountObject = entryObject.take("amount").toObject();
                            entry.amount.assetId     = amountObject.take("asset_id").toInt();
                            QJsonValue amountValue   = amountObject.take("amount");
                            if( amountValue.isString())
                            {
                                entry.amount.amount  = amountValue.toString().toULongLong();
                            }
                            else
                            {
                                entry.amount.amount  = QString::number(amountValue.toDouble(),'g',10).toULongLong();
                            }

                            QJsonArray runningBalanceArray  = entryObject.take("running_balances").toArray().at(0).toArray().at(1).toArray();
                            for( int k = 0; k < runningBalanceArray.size(); k++)
                            {
                                QJsonObject amountObject2    = runningBalanceArray.at(k).toArray().at(1).toObject();
                                AssetAmount assetAmount;
                                assetAmount.assetId = amountObject2.take("asset_id").toInt();
                                QJsonValue amountValue2   = amountObject2.take("amount");
                                if( amountValue2.isString())
                                {
                                    assetAmount.amount  = amountValue2.toString().toULongLong();
                                }
                                else
                                {
                                    assetAmount.amount  = QString::number(amountValue2.toDouble(),'g',10).toULongLong();
                                }
                                entry.runningBalances.append(assetAmount);
                            }

                            transactionInfo.entries.append(entry);
                        }

                        QJsonObject object5         = object.take("fee").toObject();
                        QJsonValue amountValue3     = object5.take("amount");
                        if( amountValue3.isString())
                        {
                            transactionInfo.fee     = amountValue3.toString().toULongLong();
                        }
                        else
                        {
                            transactionInfo.fee     = QString::number(amountValue3.toDouble(),'g',10).toULongLong();
                        }
                        transactionInfo.feeId       = object5.take("asset_id").toInt();

                        transactionsInfoVector.append(transactionInfo);
                    }

                    transactionsMap.insert(accountName,transactionsInfoVector);
                }
            }
        }
    }
}

void CDC::scanLater()
{
    QTimer::singleShot(10000,this,SLOT(scan()));
}

void CDC::updateAccountContractBalance(QString accountAddress, QString contractAddress)
{
    if( addressMap.keys().size() < 1) return;
    postRPC( toJsonFormat( "id_contract_call_offline_balanceOf+" + contractAddress + "+" + accountAddress, "contract_call_offline",
                           QStringList() << contractAddress << addressMap.keys().at(0) << "balanceOf" << accountAddress
                           ));
}

void CDC::updateAllContractBalance()
{
    QStringList keys = CDC::getInstance()->addressMap.keys();
    QStringList contracts = ERC20TokenInfoMap.keys();
    foreach (QString key, keys)
    {
        foreach (QString contract, contracts)
        {
            updateAccountContractBalance(CDC::getInstance()->addressMap.value(key).ownerAddress,contract);
        }
    }
}

void CDC::updateERC20TokenInfo(QString contractAddress)
{
    if( addressMap.keys().size() < 1) return;
    postRPC( toJsonFormat( "id_contract_call_offline_state+" + contractAddress, "contract_call_offline", QStringList() << contractAddress
                           << addressMap.keys().at(0) << "state" << ""
                           ));
}

void CDC::updateAllToken()
{
    configFile->beginGroup("/AddedContractToken");
    QStringList keys = configFile->childKeys();
    foreach (QString key, keys)
    {
        updateERC20TokenInfo(key);
    }
    configFile->endGroup();
}

void CDC::collectContractTransactions(QString contractAddress)
{
    if( ERC20TokenInfoMap.keys().contains(contractAddress))
    {
        int fromBlockHeight = ERC20TokenInfoMap.value(contractAddress).collectedBlockHeight + 1;
        int toBlockHeight = currentBlockHeight;

        qDebug() << contractAddress << ", " << fromBlockHeight << ", " << toBlockHeight;
        if( fromBlockHeight > toBlockHeight)    return;
        postRPC( toJsonFormat( "id_blockchain_list_contract_transaction_history+" + contractAddress + "+" + QString::number(toBlockHeight),
                               "blockchain_list_contract_transaction_history",
                               QStringList() << QString::number(fromBlockHeight) << QString::number(toBlockHeight)
                               << contractAddress << "14"));
    }
}

void CDC::collectContracts()
{
    foreach (QString key, ERC20TokenInfoMap.keys())
    {
        collectContractTransactions(key);
    }
}

void CDC::scan()
{
    postRPC( toJsonFormat( "id_scan", "scan", QStringList() << "0"));
}



void CDC::quit()
{
    if (proc)
    {
        proc->close();
        qDebug() << "proc: close";
        delete proc;
        proc = NULL;
    }
}

void CDC::startBlockChain(QStringList &params)
{
    qDebug() << "cdc.exe " << params.join(" ");
    this->proc->start("cdc.exe", params);
}

void CDC::updateJsonDataMap(QString id, QString data)
{
    mutexForJsonData.lock();

    jsonDataMap.insert( id, data);
    emit jsonDataUpdated(id);

    mutexForJsonData.unlock();

}

QString CDC::jsonDataValue(QString id)
{

    mutexForJsonData.lock();

    QString value = jsonDataMap.value(id);

    mutexForJsonData.unlock();

    return value;
}

double CDC::getPendingAmount(QString name)
{
    mutexForConfigFile.lock();
    if( !pendingFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "pending.dat not exist";
        return 0;
    }
    QString str = QByteArray::fromBase64( pendingFile->readAll());
    pendingFile->close();
    QStringList strList = str.split(";");
    strList.removeLast();

    mutexForConfigFile.unlock();

    double amount = 0;
    foreach (QString ss, strList)
    {
        QStringList sList = ss.split(",");
        if( sList.at(1) == name)
        {
            amount += sList.at(2).toDouble() + sList.at(3).toDouble();
        }
    }

    return amount;
}

QString CDC::getPendingInfo(QString id)
{
    mutexForConfigFile.lock();
    if( !pendingFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "pending.dat not exist";
        return 0;
    }
    QString str = QByteArray::fromBase64( pendingFile->readAll());
    pendingFile->close();
    QStringList strList = str.split(";");
    strList.removeLast();

    mutexForConfigFile.unlock();

    QString info;
    foreach (QString ss, strList)
    {
        QStringList sList = ss.split(",");
        if( sList.at(0) == id)
        {
            info = ss;
            break;
        }
    }

    return info;
}

int CDC::getDelegateIndex(QString delegateName)
{
    mutexForDelegateList.lock();
    QStringList singleList = CDC::getInstance()->delegateList.filter( "\"name\":\"" + delegateName + "\"");
    mutexForDelegateList.unlock();
    if( singleList.size() == 0) return -1;  // 代理list中没有时 返回 -1
    int pos = singleList.at(0).indexOf("\"index\":") + 8;
    int rank = singleList.at(0).mid(pos).toInt();
    return rank;
}

QString doubleTo5Decimals(double number)
{
        QString num = QString::number(number,'f', 5);
        int pos = num.indexOf('.') + 6;
        return num.mid(0,pos);
}

QString CDC::registerMapValue(QString key)
{
    mutexForRegisterMap.lock();
    QString value = registerMap.value(key);
    mutexForRegisterMap.unlock();

    return value;
}

void CDC::registerMapInsert(QString key, QString value)
{
    mutexForRegisterMap.lock();
    registerMap.insert(key,value);
    mutexForRegisterMap.unlock();
}

int CDC::registerMapRemove(QString key)
{
    mutexForRegisterMap.lock();
    int number = registerMap.remove(key);
    mutexForRegisterMap.unlock();
    return number;
}

QString CDC::balanceMapValue(QString key)
{
    mutexForBalanceMap.lock();
    QString value = balanceMap.value(key);
    mutexForBalanceMap.unlock();

    return value;
}

void CDC::balanceMapInsert(QString key, QString value)
{
    mutexForBalanceMap.lock();
    balanceMap.insert(key,value);
    mutexForBalanceMap.unlock();
}

int CDC::balanceMapRemove(QString key)
{
    mutexForBalanceMap.lock();
    int number = balanceMap.remove(key);
    mutexForBalanceMap.unlock();
    return number;
}

TwoAddresses CDC::addressMapValue(QString key)
{
    mutexForAddressMap.lock();
    TwoAddresses value = addressMap.value(key);
    mutexForAddressMap.unlock();

    return value;
}

void CDC::addressMapInsert(QString key, TwoAddresses value)
{
    mutexForAddressMap.lock();
    addressMap.insert(key,value);
    mutexForAddressMap.unlock();
}

int CDC::addressMapRemove(QString key)
{
    mutexForAddressMap.lock();
    int number = addressMap.remove(key);
    mutexForAddressMap.unlock();
    return number;
}

bool CDC::rpcReceivedOrNotMapValue(QString key)
{
    mutexForRpcReceiveOrNot.lock();
    bool received = rpcReceivedOrNotMap.value(key);
    mutexForRpcReceiveOrNot.unlock();
    return received;
}

void CDC::rpcReceivedOrNotMapSetValue(QString key, bool received)
{
    mutexForRpcReceiveOrNot.lock();
    rpcReceivedOrNotMap.insert(key, received);
    mutexForRpcReceiveOrNot.unlock();
}


void CDC::appendCurrentDialogVector( QWidget * w)
{
    currentDialogVector.append(w);
}

void CDC::removeCurrentDialogVector( QWidget * w)
{
    currentDialogVector.removeAll(w);
}

void CDC::hideCurrentDialog()
{
    foreach (QWidget* w, currentDialogVector)
    {
        w->hide();
    }
}

void CDC::showCurrentDialog()
{
    foreach (QWidget* w, currentDialogVector)
    {
        qDebug() << "showCurrentDialog :" << w;
        w->show();
        w->move( mainFrame->pos());  // lock界面可能会移动，重新显示的时候要随之移动
    }
}

void CDC::resetPosOfCurrentDialog()
{
    foreach (QWidget* w, currentDialogVector)
    {
        w->move( mainFrame->pos());
    }
}

void CDC::initWorkerThreadManager()
{
    qDebug() << "initWorkerThreadManager " << QThread::currentThreadId();
    if( workerManager)
    {
        delete workerManager;
    }
    if( threadForWorkerManager)
    {
        delete threadForWorkerManager;
    }

    threadForWorkerManager = new QThread;
    threadForWorkerManager->start();
    workerManager = new WorkerThreadManager;
    workerManager->moveToThread(threadForWorkerManager);
    connect(this, SIGNAL(rpcPosted(QString)), workerManager, SLOT(processRPCs(QString)));

}

void CDC::destroyWorkerThreadManager()
{
    qDebug() << "destroyWorkerThreadManager " << QThread::currentThreadId();
    if( workerManager)
    {
        workerManager->deleteLater();
        workerManager = NULL;

        if( threadForWorkerManager)
        {
            threadForWorkerManager->deleteLater();
            threadForWorkerManager = NULL;
        }
    }

}

void CDC::postRPC(QString cmd)
{
    if( rpcReceivedOrNotMap.contains( cmd))
    {
        if( rpcReceivedOrNotMap.value( cmd) == true)
        {
            rpcReceivedOrNotMapSetValue( cmd, false);
            emit rpcPosted(cmd);
        }
        else
        {
            // 如果标识为未返回 则不重复排入事件队列
            return;
        }
    }
    else
    {
        rpcReceivedOrNotMapSetValue( cmd, false);
        emit rpcPosted(cmd);
    }

}

double roundDown(double decimal, int precision)
{
    int precisonFigureNum   = QString::number( precision, 'g', 15).count() - 1;

    double result = QString::number(decimal,'f',precisonFigureNum).toDouble();
    if( result > decimal)
    {
        if( precision == 0)     precision = 1;
        result = result - 1.0 / precision;
    }

    return result;
}

bool isExistInWallet(QString strName)
{
    mutexForAddressMap.lock();
    for (QMap<QString,TwoAddresses>::const_iterator i = CDC::getInstance()->addressMap.constBegin(); i != CDC::getInstance()->addressMap.constEnd(); ++i)
    {
        if(i.key() == strName)
        {
            mutexForAddressMap.unlock();
            return true;
        }
    }
    mutexForAddressMap.unlock();
    return false;
}

QString removeLastZeros(QString number)     // 去掉小数最后面的0
{
    if( !number.contains('.'))  return number;

    while (number.endsWith('0'))
    {
        number.chop(1);
    }

    if( number.endsWith('.'))   number.chop(1);

    return number;
}

QString getBigNumberString(unsigned long long number, unsigned long long precision)
{
    QString str = QString::number(number);
    int size = QString::number(precision).size() - 1;
    if( size < 0)  return "";
    if( size == 0) return str;

    if( str.size() > size)
    {
        str.insert(str.size() - size, '.');
    }
    else
    {
        // 前面需要补0
        QString zeros;
        zeros.fill('0',size - str.size() + 1);
        str.insert(0,zeros);
        str.insert(1,'.');
    }

    return removeLastZeros(str);
}

bool checkAddress(QString address)
{
    if( address.size() == 33 || address.size() == 34)
    {
        if( address.startsWith("G"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
