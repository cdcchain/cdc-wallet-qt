// Copyright (c) 2017-2018 The CDC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include "consolewidget.h"
#include "ui_consolewidget.h"
#include "blockchain.h"
#include "rpcthread.h"
#include "pubic_define.h"
#include <QTextCodec>
#include <QPainter>
#include <QKeyEvent>
#include <QDesktopWidget>

ConsoleWidget::ConsoleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConsoleWidget)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);

    ui->consoleLineEdit->installEventFilter(this);

    connect( CDC::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    ui->closeBtn->setStyleSheet("QToolButton{background-image:url(:/pic/pic2/close4.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding;border-style: flat;}");

    ui->consoleLineEdit->setFocus();

    ui->checkBox->setStyleSheet("QCheckBox::indicator{ image:url(:/pic/pic2/checkBox_unchecked.png); }"
                                    "QCheckBox::indicator:checked{ image:url(:/pic/cplpic/checkBox_checked.png); }");

//    ui->consoleBrowser->setText(QString::fromLocal8Bit("<body><font color=red>警告： 已有骗子通过要求用户在此输入指令以盗取钱包。不要在没有完全理解命令规范时使用控制台。</font><p></p><p></p></body>"));

    ui->consoleBrowser->setText( "<body><font color=red>" + tr("Warning: Some fraud may ask the user to enter the instruction here to steal the coins. Do not use the console without fully understanding the commands' specification.") + "</font><p></p><p></p></body>" );
    ui->clearBtn->setStyleSheet("QToolButton{background-color:rgb("STR_BUTTON_COLOR");color:#ffffff;border:none;border-radius:16px;}QToolButton:hover{background-color:rgb("STR_BUTTON_COLOR");}QToolButton:disabled{background-color:#cecece;}");
}

ConsoleWidget::~ConsoleWidget()
{
    delete ui;
    CDC::getInstance()->consoleWidget = NULL;
}

void ConsoleWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(40,28,64),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(40,28,64),Qt::SolidPattern));
    painter.drawRect(0,0,628,50);

    painter.setBrush(Qt::transparent);
    painter.setPen(QColor(51,38,87));
    painter.drawRect(QRect(0,50,627,417));
}

bool ConsoleWidget::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == ui->consoleLineEdit)
    {
        if(e->type() == QEvent::KeyPress)
        {
            QKeyEvent* event = static_cast<QKeyEvent*>(e);
            if( event->key() == Qt::Key_Up)
            {
                cmdIndex--;
                if( cmdIndex >= 0 && cmdIndex <= cmdVector.size() - 1)
                {
                    ui->consoleLineEdit->setText(cmdVector.at(cmdIndex));
                }

                if( cmdIndex < 0)
                {
                    cmdIndex = 0;
                }

            }
            else if( event->key() == Qt::Key_Down)
            {
                cmdIndex++;
                if( cmdIndex >= 0 && cmdIndex <= cmdVector.size() - 1)
                {
                    ui->consoleLineEdit->setText(cmdVector.at(cmdIndex));
                }

                if( cmdIndex > cmdVector.size() - 1)
                {
                    cmdIndex = cmdVector.size() - 1;
                }

            }
        }

    }

    return QWidget::eventFilter(watched,e);
}

void ConsoleWidget::on_closeBtn_clicked()
{
    close();
}


void ConsoleWidget::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
     {
          mouse_press = true;
          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
          move_point = event->pos();;
     }
}

void ConsoleWidget::mouseMoveEvent(QMouseEvent *event)
{
    //若鼠标左键被按下
    if(mouse_press)
    {
        //鼠标相对于屏幕的位置
        QPoint move_pos = event->globalPos();

        //移动主窗体位置
        this->move(move_pos - move_point);
    }
}

void ConsoleWidget::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void ConsoleWidget::on_consoleLineEdit_returnPressed()
{
    if( !ui->consoleLineEdit->text().simplified().isEmpty())
    {
        cmdVector.removeAll(ui->consoleLineEdit->text());
        cmdVector.append(ui->consoleLineEdit->text());
        cmdIndex = cmdVector.size();
    }
    else
    {
        if( ui->checkBox->isChecked())  return;     // 输入为空且为rpc模式时   直接返回
    }

    if( ui->checkBox->isChecked())
    {
        QString str = ui->consoleLineEdit->text();
        QStringList paramaters = str.split(' ');
        QString command = paramaters.at(0);
        paramaters.removeFirst();
        if( paramaters.isEmpty())  paramaters << "";

        CDC::getInstance()->postRPC( toJsonFormat( "console_" + str, command, paramaters ));

        ui->consoleLineEdit->clear();
        return;
    }

    QString str = ui->consoleLineEdit->text() + '\n';
    CDC::getInstance()->write(str);
    QString result = CDC::getInstance()->read();
    ui->consoleBrowser->append(">>>" + str);
    ui->consoleBrowser->append(result);
    ui->consoleLineEdit->clear();
}

void ConsoleWidget::jsonDataUpdated(QString id)
{
    if( id.startsWith("console_"))
    {
        ui->consoleBrowser->append(">>>" + id.mid(8));
        ui->consoleBrowser->append( CDC::getInstance()->jsonDataValue(id));
        ui->consoleBrowser->append("\n");
        return;
    }
}

void ConsoleWidget::on_clearBtn_clicked()
{
    ui->consoleLineEdit->clear();
    ui->consoleBrowser->clear();
}
