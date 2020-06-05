#include "room.h"

room::room(QObject *parent) : QObject(parent)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    //temperature=qrand()%120/(10.0)+18.0;
    temperature=25.0;
    nature=0;
    times = new QTimer();
    connect(times,SIGNAL(timeout()),this,SLOT(sig()));
}

float room::gettemp()
{
    return this->temperature;
}

void room::settemp(float x)
{
    this->temperature=x;
}

void room::settempNature(float x)
{
    if(nature==1)
        this->temperature=x;
    if(fabs(x-avgTemp)<0.00001)
    {
        nature=1;
    }
    else
    {
        nature=0;
    }
}

void room::startTime()
{
    times->start(4000);
}

void room::endTime()
{
    times->stop();
}

bool room::getTimeState()
{
    return times->isActive();
}

void room::sig()
{
    emit timeover();
}
