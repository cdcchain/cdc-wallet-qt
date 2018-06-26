// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.



#ifndef CDC_H
#define CDC_H
#include <QObject>
#include <QMap>
#include <QSettings>
#include <QFile>
#include <QProcess>
#include <QMutex>
#include <QDialog>

#define ASSET_NAME "CDC"
#define WALLET_VERSION "0.1.2 test"           // 版本号
#define AUTO_REFRESH_TIME 1000           // 自动刷新时间(ms)

//  密码输入错误5次后 锁定一段时间 (秒)
#define PWD_LOCK_TIME  7200
#define RPC_PORT 53016


class QTimer;
class QFrame;
class WorkerThreadManager;
class ConsoleWidget;

static QMutex mutexForJsonData;
static QMutex mutexForPending;
static QMutex mutexForConfigFile;
static QMutex mutexForMainpage;
static QMutex mutexForPendingFile;
static QMutex mutexForDelegateList;
static QMutex mutexForRegisterMap;
static QMutex mutexForBalanceMap;
static QMutex mutexForAddressMap;
static QMutex mutexForRpcReceiveOrNot;


typedef QMap<int,unsigned long long>  AssetBalanceMap;
typedef QMap<QString,unsigned long long>  ContractBalanceMap;
struct AssetInfo
{
    int id;
    QString symbol;
    QString owner;
    QString name;
    QString description;
    unsigned long long precision;
    unsigned long long maxSupply;
    unsigned long long currentSupply;
    QString registrationDate;
};
struct AssetAmount
{
    int assetId;
    unsigned long long amount;
};
struct ERC20TokenInfo
{
    QString contractAddress;
    QString symbol;
    QString name;
    QString admin;
    unsigned long long precision = 1;
    unsigned long long totalSupply = 0;
    int collectedBlockHeight = 1;   // 该合约已经采集到的高度
};

struct Entry
{
    QString fromAccount;
    QString toAccount;
    AssetAmount amount;
    QString memo;
    QVector<AssetAmount>    runningBalances;
};
struct TransactionInfo
{
    QString trxId;
    bool isConfirmed;
    bool isMarket;
    bool isMarketCancel;
    int blockNum;
    QVector<Entry>  entries;
//    QString fromAaccount;
//    QString toAccount;
//    double amount;      //  交易金额
//    int assetId;        //  交易金额的资产类型
//    QString memo;
//    AssetBalanceMap runningBalances;
    unsigned long long fee;
    int    feeId;
    QString timeStamp;
};
typedef QVector<TransactionInfo>  TransactionsInfoVector;

struct ContractTransaction
{
    QString contractAddress;
    QString trxId;
    int blockNum;
    QString fromAddress;
    QString toAddress;
    unsigned long long amount;
    QString timeStamp;
    unsigned long long fee;
};
typedef QVector<ContractTransaction>  ContractTransactionVector;

struct TwoAddresses     // owner_address 和 active_address
{
    QString ownerAddress;
    QString activeAddress;
};

class CDC : public QObject
{
    Q_OBJECT
public:
    ~CDC();
    static CDC*   getInstance();
    qint64 write(QString);
    void quit();
    void startBlockChain(QStringList &params);
    QString read();
    QProcess* proc;
    int lockMinutes;   // 自动锁定时间
    bool notProduce;   // 是否产块/记账
    bool minimizeToTray;  // 是否最小化到托盘
    bool closeToMinimize; // 是否点击关闭最小化
    bool resyncNextTime;    // 下次启动时是否重新同步
    bool firstUse;      // 如果是第一次使用  弹个warningdialog
    QString language;   // 语言
    bool needToScan;   // 在当前scan完成后是否还需要scan
    QString currentAccount; // 保存当前账户  切换页面的时候默认选择当前账户
    unsigned long long transactionFee;
    ConsoleWidget* consoleWidget;

    bool stopping;      // 正在关闭exe  停止处理rpc

    QMap<QString,QString> balanceMap;
    QMap<QString,TwoAddresses> addressMap;
    QMap<QString,QString> registerMap;
    QStringList delegateAccountList;
    QStringList delegateList;
    bool hasDelegateSalary;
    QMap<QString,double> delegateSalaryMap;
    QMap<QString,bool> rpcReceivedOrNotMap;  // 标识rpc指令是否已经返回了，如果还未返回则忽略

    TwoAddresses getAddress(QString);
    bool    isMyAddress(QString address);
    QString addressToName(QString address);
    QString getBalance(QString);
    QString getRegisterTime(QString);
    void deleteAccountInConfigFile(QString);
    void updateJsonDataMap(QString id, QString data);
    QString jsonDataValue(QString id);
    double getPendingAmount(QString name);
    QString getPendingInfo(QString id);
    int getDelegateIndex(QString delegateName);

    QString registerMapValue(QString key);
    void registerMapInsert(QString key, QString value);
    int registerMapRemove(QString key);
    QString balanceMapValue(QString key);
    void balanceMapInsert(QString key, QString value);
    int balanceMapRemove(QString key);
    TwoAddresses addressMapValue(QString key);
    void addressMapInsert(QString key, TwoAddresses value);
    int addressMapRemove(QString key);
    bool rpcReceivedOrNotMapValue(QString key);
    void rpcReceivedOrNotMapSetValue(QString key, bool received);

    void appendCurrentDialogVector(QWidget*);
    void removeCurrentDialogVector(QWidget *);
    void hideCurrentDialog();
    void showCurrentDialog();
    void resetPosOfCurrentDialog();

    void initWorkerThreadManager();
    void destroyWorkerThreadManager();
    void postRPC(QString cmd);

    void getContactsFile();  // contacts.dat 改放到数据路径

    QVector<QWidget*> currentDialogVector;  // 保存不属于frame的dialog
                                            // 为的是自动锁定的时候hide这些dialog

    QSettings *configFile;
//    void loadAccountInfo();

    QString appDataPath;
    QString walletConfigPath;

    QFile* contactsFile;
    QFile* pendingFile;

    QThread* threadForWorkerManager;

    QDialog* currentDialog;  // 如果不为空 则指向当前最前面的不属于frame的dialog
                             // 为的是自动锁定的时候hide该dialog

    QFrame* mainFrame = NULL; // 指向主窗口的指针

    int currentPort;          // 当前rpc 端口

    QString localIP;   // 保存 peerinfo 获得的本机IP和端口


    QMap<QString,AssetBalanceMap> accountBalanceMap;
    void parseBalance();

    QMap<int,AssetInfo>  assetInfoMap;
    void parseAssetInfo();
    int getAssetId(QString symbol);

    QMap<QString,TransactionsInfoVector> transactionsMap;   // key是 "账户名-资产符号" 形式
    void parseTransactions(QString result, QString accountName = "ALL");

    void scanLater();


    // 合约资产
    QMap<QString,ContractBalanceMap> accountContractBalanceMap;
    void updateAccountContractBalance(QString accountAddress, QString contractAddress);
    void updateAllContractBalance();

    QMap<QString,ERC20TokenInfo>  ERC20TokenInfoMap;
    void updateERC20TokenInfo(QString contractAddress);
    void updateAllToken();

    int currentBlockHeight = 0;     // info 获得的当前区块高度
    QString currentTokenAddress;

    QMap<QString,ContractTransactionVector> accountContractTransactionMap;  // key是 "账户地址-合约地址" 形式
    void collectContractTransactions(QString contractAddress);
    void collectContracts();

private slots:
    void scan();


signals:
    void started();

    void jsonDataUpdated(QString);

    void rpcPosted(QString cmd);

private:

    CDC();
    static CDC* goo;
    QMap<QString,QString> jsonDataMap;   //  各指令的id,各指令的返回
    WorkerThreadManager* workerManager;  // 处理rpc worker thread


    void getSystemEnvironmentPath();
    void changeToWalletConfigPath();     // 4.2.2后config pending log 等文件移动到 APPDATA路径

    class CGarbo // 它的唯一工作就是在析构函数中删除CSingleton的实例
    {
    public:
        ~CGarbo()
        {
            if (CDC::goo)
                delete CDC::goo;
        }
    };
    static CGarbo Garbo; // 定义一个静态成员，在程序结束时，系统会调用它的析构函数
};

QString doubleTo5Decimals(double number);
double roundDown(double decimal, int precision = 0);        // 根据精度 向下取"整"
QString removeLastZeros(QString number);        // qstring::number() 对小数的处理有问题  使用std::to_string() 然后把后面的0去掉
QString getBigNumberString(unsigned long long number,unsigned long long precision);
bool   checkAddress(QString address);
bool isExistInWallet(QString);

#endif // CDC_H

