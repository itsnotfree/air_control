#include "mainmodel.h"

mainmodel::mainmodel(QObject *parent) : QObject(parent)
{
    aroom=new room();
    this->checkonline= new QTimer();
    socket=socketSingle::getSocket();
    connect(socket,SIGNAL(readyRead()),this,SLOT(revData()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(closed()));
    connect(this->checkonline,SIGNAL(timeout()),this,SLOT(stopall()));
    sendMux=0;
    tempMux=0;
    wind=0;
}

void mainmodel::setall(int workMode, float lowTemp, float highTemp,int worktemp)
{
    this->workMode=workMode;
    this->lowTemp=lowTemp;
    this->highTemp=highTemp;
    this->worktemp=worktemp;

    changeworktemp(this->worktemp);
    if(!aroom->getTimeState())
    aroom->startTime();
    connect(aroom,SIGNAL(timeover()),this,SLOT(normalchange()));

}

room *mainmodel::getroom()
{
    return aroom;
}

void mainmodel::sendwind(int wind) //发送改变请求
{
    QString a="wind/"+QString::number(wind);
    this->wind=wind;
    socket->write(a.toLatin1());
}

/**
 * @brief mainmodel::changeworktemp
 * 当工作温度改变时，进行判断并开始判断5种情况
 * @param worktemp
 * 工作温度
 */
void mainmodel::changeworktemp(float worktemp)
{
    this->worktemp=worktemp;
    if(fabs(aroom->gettemp()-worktemp)>0.01) //当工作温度和房间温度不相等时
    {
            if(sendMux==0&&aroom->gettemp()<worktemp&&workMode==1) //满足制热条件
            {
                emit changestate(1);
                sendMux=2;
                QString a="request/"+QString::number(this->aroom->gettemp())+"/"+QString::number(worktemp);
                socket->write(a.toLatin1());
                this->checkonline->setSingleShot(true);
                this->checkonline->start(3000);
                //qDebug()<<"制热";

            }
            else if(sendMux==2&&aroom->gettemp()<worktemp&&workMode==1)
            {

            }
            else if(sendMux==0&&aroom->gettemp()>worktemp&&workMode==0) //满足制冷条件
            {
                emit changestate(1);
                sendMux=2;
                QString a="request/"+QString::number(this->aroom->gettemp())+"/"+QString::number(worktemp);
                socket->write(a.toLatin1());
                this->checkonline->setSingleShot(true);
                this->checkonline->start(3000);
                //qDebug()<<"制冷";
            }
            else if(sendMux==2&&aroom->gettemp()>worktemp&&workMode==0)
            {

            }
            else  if(sendMux==2&&workMode==0&&aroom->gettemp()<worktemp)
                {
                    qDebug()<<"进入了1";
                    sendMux=0;
                    QString a="requestEnd";
                    socket->write(a.toLatin1());
                    emit changestate(2);
                    aroom->endTime();
                    aroom->startTime();
                }
                else if(sendMux==2&&workMode==1&&aroom->gettemp()>worktemp)
                {
                    qDebug()<<"进入了";
                    sendMux=0;
                    QString a="requestEnd";
                    socket->write(a.toLatin1());
                    emit changestate(2);
                    aroom->endTime();
                    aroom->startTime();
                }
                else
                {
                    qDebug()<<"没进入";
                    sendMux=0;
                }

    }
}

/**
 * @brief mainmodel::revData
 * 接受数据包并进行分析判断
 */
void mainmodel::revData()
{
    QString datas = socket->readAll();

    QString patternRequest("(.*)request(.*)");
    QString patternCost("(.*)cost(.*)");
    QString patternAnswer("(.*)answer(.*)");
    QString patternAcceptWind("(.*)acceptWind(.*)");

    QRegExp rx1(patternRequest);
    QRegExp rx2(patternCost);
    QRegExp rx3(patternAnswer);
    QRegExp rx4(patternAcceptWind);


    int index;
    if(rx1.exactMatch(datas))
    {
         index=datas.indexOf("request");
         if(index!=0)
         datas.insert(index, ",");
    }
    if(rx2.exactMatch(datas))
    {
         index=datas.indexOf("cost");
         if(index!=0)
         datas.insert(index, ",");
    }
    if(rx3.exactMatch(datas))
    {
         index=datas.indexOf("answer");
         if(index!=0)
         datas.insert(index, ",");
    }
    if(rx4.exactMatch(datas))
    {
         index=datas.indexOf("acceptWind");
         if(index!=0)
         datas.insert(index, ",");
    }
    QStringList list=datas.split(",");
    for(int i=0;i<list.length();i++)
    {
            splite(QString(list[i]));
    }
}


/**
 * @brief mainmodel::splite
 * 防止沾包进行二次拆分
 * @param x
 * 某个准确无误的信息串
 */
void mainmodel::splite(QString x)
{
    QStringList list = x.split("/");
    qDebug()<<"1"<<x;
    if(QString::compare(QString(list[0]),"cost")==0)
    {
        //ui->textBrowser->append("enegy"+QString(list[1])+"\ncost:"+QString(list[2]));
        emit showcost(QString(list[1]),QString(list[2]));
    }
    if(QString::compare(QString(list[0]),"request")==0)
    {
        QString a= "answer/"+QString::number(aroom->gettemp())+"/"+QString::number(worktemp)+"/"+QString::number(wind);
        socket->write(a.toLatin1());
    }
    if(QString::compare(QString(list[0]),"answer")==0)
    {
        this->checkonline->stop();
        changeTemp();
        tempMux=0;
    }
    if(QString::compare(QString(list[0]),"acceptWind")==0)
    {
        emit changewind();
    }
}



/**
 * @brief mainmodel::changeTemp
 * 改变温度
 */
void mainmodel::changeTemp()
{
    float x=0;
    switch(wind)
    {
        case 0:
            x=0.1; break;
        case 1:
            x=0.2; break;
        case 2:
            x=0.3; break;
    }
    if(this->workMode==0)
        transTemp(1,x);

    if(this->workMode==1)
        transTemp(0,x);

    if(sendMux==2&&check()==false)
        QTimer::singleShot(3000,this,SLOT(sendReq())); //降降速
}

/**
 * @brief mainmodel::check
 * 判断室温是否到达工作温度 及善后工作
 * @return
 * 是返回1 否返回0
 */
bool mainmodel::check()
{
    if(workMode==0&&(aroom->gettemp()-worktemp)<=0.01&&sendMux==2)
    {
        QString a="requestEnd";
        socket->write(a.toLatin1());
        emit changestate(2);
        aroom->endTime();
        aroom->startTime();
        sendMux=0;
        return true;
    }
    if(workMode==1&&(aroom->gettemp()-worktemp)>=0.01&&sendMux==2)
    {
        QString a="requestEnd";
        socket->write(a.toLatin1());
        emit changestate(2);
        aroom->endTime();
        aroom->startTime();
        sendMux=0;
        return true;
    }
    return false;
}

void mainmodel::transTemp(int sym,float num)
{
    if(sym==0)
    aroom->settemp(aroom->gettemp()+num);
    else
    aroom->settemp(aroom->gettemp()-num);

    emit changeroomtemp(aroom->gettemp());
}


bool mainmodel::tcpstate()
{
    if(socket)
    {
        return 1;
    }
    else
       {
        return 0;
    }
}


/**
 * @brief mainmodel::interrept
 * 手动断开连接
 */
void mainmodel::interrept()
{
    socket->abort();
}

void mainmodel::sendReq()
{
    QString a="request/"+QString::number(this->aroom->gettemp())+"/"+QString::number(worktemp);
    socket->write(a.toLatin1());
    this->checkonline->start();
}

/**
 * @brief mainmodel::normalchange
 *房间温度的改变
 */
void mainmodel::normalchange()
{
    if(sendMux==0)
    {
        emit changestate(2);
        if(workMode==0&&(aroom->gettemp()-worktemp)<0.99) //制冷模式 房间温度高 但没超过1度
        {
            //if(!(fabs(this->trytransTemp()-room::avgTemp)<0.00001))
            if(!(fabs(aroom->gettemp()-room::avgTemp)<0.00001))
            transTemp(0,0.1);
        }
        else if(workMode==0&&(aroom->gettemp()-worktemp)>0.99) //制冷模式 房间温度高 超过1度
        {
            if(tempMux==0)
            changeworktemp(this->worktemp);
            else
            {
            if(!(fabs(aroom->gettemp()-room::avgTemp)<0.00001))
            transTemp(0,0.1);
            }
        }
        else if(workMode==0&&(worktemp-aroom->gettemp())>0.00) //制冷模式 房间温度低
        {
            transTemp(0,0.1);
        }

        else if(workMode==1&&(aroom->gettemp()-worktemp)>0.00) //制热模式 房间温度高
        {
            transTemp(1,0.1);
        }
        else if(workMode==1&&(worktemp-aroom->gettemp())<0.99) //制热模式 房间温度低 没超过过1度
        {
            if(!(fabs(aroom->gettemp()-room::avgTemp)<0.00001))
            transTemp(1,0.1);
        }
        else if(workMode==1&&(worktemp-aroom->gettemp())>0.99) //制热模式 房间温度低 超过1度
        {
            if(tempMux==0)
            changeworktemp(this->worktemp);
            else
            {
                if(!(fabs(aroom->gettemp()-room::avgTemp)<0.00001))
                    transTemp(1,0.1);
            }
        }

    }
}

/**
 * @brief mainmodel::closed
 * 断开tcp后的操作
 */
void mainmodel::closed()
{
   disconnect(socket,SIGNAL(readyRead()),this,SLOT(revData()));
   emit this->changewid();
}

void mainmodel::stopall()
{
    qDebug()<<"停机";
    emit changestate(2);
    aroom->endTime();
    aroom->startTime();
    sendMux=0;
    tempMux=1;
}
