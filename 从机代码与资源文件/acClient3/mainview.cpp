#include "mainview.h"
#include "ui_mainview.h"

mainView::mainView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainView)
{
    ui->setupUi(this);
    x= new mainmodel();
    ui->pushButton_off->setStyleSheet("QPushButton#pushButton_off:hover{border-image: url(://new//prefix1//pic//an_kg.jpg);}QPushButton#pushButton_off{border-image: url(://new//prefix1//pic//kg.jpg);}");
    clickMux=0;
    wind=0;
}

mainView::~mainView()
{
    delete ui;
}

/**
 * @brief mainView::setState
 * 初始状态设置
 * @param workMode
 * @param lowTemp
 * @param highTemp
 */
void mainView::setState(int workMode, float lowTemp, float highTemp)
{

    ui->lcdNumber_low->display(lowTemp);
    ui->lcdNumber_high->display(highTemp);

    ui->lcdNumber_roomtep->display(x->getroom()->gettemp());

    if(workMode==0)
    {
    ui->lcdNumber_worktep->display(22);
    ui->showworkmode->setStyleSheet("border-image: url(:/new/prefix1/pic/zl.jpg);");
    x->setall(workMode, lowTemp, highTemp,22);
    }
    if(workMode==1)
    {
    ui->lcdNumber_worktep->display(28);
    ui->showworkmode->setStyleSheet("border-image: url(:/new/prefix1/pic/zr.jpg);");
    x->setall(workMode, lowTemp, highTemp,28);
    }

    //connect(ui->workTemp,SIGNAL(valueChanged(double)),this,SLOT(workTempChanged(double)));

    connect(x,SIGNAL(changeroomtemp(float)),this,SLOT(freshtoomtemp(float)));
    connect(x,SIGNAL(showcost(QString,QString)),this,SLOT(showcost(QString,QString)));
    connect(x,SIGNAL(changewind()),this,SLOT(changewind()));
    connect(x,SIGNAL(changewid()),this,SLOT(changewid()));
    connect(x,SIGNAL(changestate(int)),this,SLOT(changestate(int)));
}


/**
 * @brief mainView::sendworkTemp
 * 到model里进行触发发送
 */
void mainView::sendworkTemp()
{
    x->changeworktemp(ui->lcdNumber_worktep->value());
    timetochange->stop();
    clickMux=0;
}


/**
 * @brief mainView::freshtoomtemp
 * 更新室温显示
 * @param x
 */
void mainView::freshtoomtemp(float x)
{
    qDebug()<<x;
    ui->lcdNumber_roomtep->display(x);
}

/**
 * @brief mainView::showcost
 * 显示温度和能量
 * @param a
 * @param b
 */
void mainView::showcost(QString a,QString b)
{
    ui->lcdNumber_en->display(a.toFloat());
    ui->lcdNumber_money->display(b.toFloat());
}


/**
 * @brief mainView::changewind
 * 改变风速显示
 */
void mainView::changewind()
{
    switch(wind)
    {
        case 0:
        ui->windspeed->setStyleSheet("border-image: url(://new//prefix1//pic//ds.jpg);");
        break;
        case 1:
        ui->windspeed->setStyleSheet("border-image: url(://new//prefix1//pic//zs.jpg);");
        break;
        case 2:
        ui->windspeed->setStyleSheet("border-image: url(://new//prefix1//pic//gs.jpg);");
        break;
    }
}
/**
 * @brief mainView::changewid
 * 转换窗口
 */
void mainView::changewid()
{
    this->close();
}


/**
 * @brief mainView::changestate
 * 改变工作状态
 * @param x
 * 0代表停机
 * 1代表工作
 */
void mainView::changestate(int x)
{
    if(x==1)
        ui->state->setStyleSheet("border-image: url(://new//prefix1//pic//gzz.jpg);");
    else
        ui->state->setStyleSheet("border-image: url(://new//prefix1//pic//xmz.jpg);");
}
/**
 * @brief mainView::closeEvent
 * 关闭此窗体触发的
 * @param event
 */
void mainView::closeEvent(QCloseEvent *event)
{
    if(x->tcpstate())
    {
        x->interrept();
    }
    emit clos();
}

/**
 * @brief mainView::on_windspeed_clicked
 * 按下发送送风请求
 */
void mainView::on_windspeed_clicked()
{
    wind=(wind+1)%3;
    x->sendwind(wind);
}

void mainView::on_pushButton_up_clicked()
{
    if(int(ui->lcdNumber_high->value())-int(ui->lcdNumber_worktep->value())>0)
    {
    qDebug()<<int(ui->lcdNumber_worktep)<<int(ui->lcdNumber_low->value());
    ui->lcdNumber_worktep->display(ui->lcdNumber_worktep->value()+1);
    if(clickMux==0)
    {
    timetochange= new QTimer();
    connect(timetochange,SIGNAL(timeout()),this,SLOT(sendworkTemp()));
    timetochange->start(1000);
    clickMux=1;
    }
    else
    {
        timetochange->stop();
        timetochange->start(1000);
    }
    }
}

void mainView::on_pushButton_down_clicked()
{
    if(int(ui->lcdNumber_worktep->value())-int(ui->lcdNumber_low->value())>0)
    {
        qDebug()<<int(ui->lcdNumber_worktep)<<int(ui->lcdNumber_low->value());
    ui->lcdNumber_worktep->display(ui->lcdNumber_worktep->value()-1);
    if(clickMux==0)
    {
    timetochange= new QTimer();
    connect(timetochange,SIGNAL(timeout()),this,SLOT(sendworkTemp()));
    timetochange->start(1000);
    clickMux=1;
    }
    else
    {
        timetochange->stop();
        timetochange->start(1000);
    }
    }
}


/**
 * @brief mainView::on_pushButton_clicked
 * 中断连接按钮
 */
void mainView::on_pushButton_off_clicked()
{
    x->interrept();

}
