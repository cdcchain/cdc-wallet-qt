#include "rightclickmenudialog.h"
#include "ui_rightclickmenudialog.h"
#include "../blockchain.h"
#include "commondialog.h"

#ifdef WIN32
#include "Windows.h"
#endif

#include <QDebug>
#include <QClipboard>

RightClickMenuDialog::RightClickMenuDialog( QString name, QWidget *parent) :
    QDialog(parent),
    accountName(name),
    ui(new Ui::RightClickMenuDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);

    ui->copyBtn->setStyleSheet("QPushButton{background-color:#ffffff;color:rgb(105,67,155);border:1px solid rgb(239,239,239);border-radius:0px;}"
                                  "QPushButton:hover{background-color:rgb(105,67,155);color:#ffffff;border-radius:0px;}"
                                  );
    ui->transferBtn->setStyleSheet("QPushButton{background-color:#ffffff;color:rgb(105,67,155);border:1px solid rgb(239,239,239);border-radius:0px;}"
                                  "QPushButton:hover{background-color:rgb(105,67,155);color:#ffffff;border-radius:0px;}"
                                  );
    ui->renameBtn->setStyleSheet("QPushButton{background-color:#ffffff;color:rgb(105,67,155);border:1px solid rgb(239,239,239);border-radius:0px;}"
                                 "QPushButton:hover{background-color:rgb(105,67,155);color:#ffffff;border-radius:0px;}"
                                 );
    ui->exportBtn->setStyleSheet("QPushButton{background-color:#ffffff;color:rgb(105,67,155);border:1px solid rgb(239,239,239);border-radius:0px;}"
                                 "QPushButton:hover{background-color:rgb(105,67,155);color:#ffffff;border-radius:0px;}"
                                 );
    ui->deleteBtn->setStyleSheet("QPushButton{background-color:#ffffff;color:rgb(105,67,155);border:1px solid rgb(239,239,239);border-radius:0px;}"
                                 "QPushButton:hover{background-color:rgb(105,67,155);color:#ffffff;border-radius:0px;}"
                                 );

    if( CDC::getInstance()->registerMapValue(name) != "NO" )  // 如果是已注册账户 不显示修改账户名选项
    {
        ui->renameBtn->hide();
        ui->exportBtn->move(0,35);
        setGeometry(0,0,116,71);
    }

}

RightClickMenuDialog::~RightClickMenuDialog()
{
    delete ui;
}

void RightClickMenuDialog::on_transferBtn_clicked()
{
    close();
    emit transferFromAccount(accountName);
}

void RightClickMenuDialog::on_renameBtn_clicked()
{
    close();
    emit renameAccount(accountName);
}

void RightClickMenuDialog::on_exportBtn_clicked()
{
    close();
    emit exportAccount(accountName);
}

bool RightClickMenuDialog::event(QEvent *event)
{
#ifdef WIN32
    // class_ameneded 不能是custommenu的成员, 因为winidchange事件触发时, 类成员尚未初始化
    static bool class_amended = false;
    if (event->type() == QEvent::WinIdChange)
    {
        HWND hwnd = reinterpret_cast<HWND>(winId());
        if (class_amended == false)
        {
            class_amended = true;
            DWORD class_style = ::GetClassLong(hwnd, GCL_STYLE);
            class_style &= ~CS_DROPSHADOW;
            ::SetClassLong(hwnd, GCL_STYLE, class_style); // windows系统函数
        }
    }
#endif
	
    return QWidget::event(event);
}

void RightClickMenuDialog::on_copyBtn_clicked()
{
    close();

    QString address = CDC::getInstance()->addressMap.value(accountName).ownerAddress;
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(address);

    CommonDialog commonDialog(CommonDialog::OkOnly);
    commonDialog.setText(tr("Copy to clipboard"));
    commonDialog.pop();
}

void RightClickMenuDialog::on_deleteBtn_clicked()
{
    close();
    emit deleteAccount(accountName);
}
