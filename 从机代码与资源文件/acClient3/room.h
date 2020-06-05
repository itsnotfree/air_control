#ifndef ROOM_H
#define ROOM_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QtGlobal>
#include <math.h>
#include <QDebug>

class room : public QObject
{
    Q_OBJECT
public:
    explicit room(QObject *parent = 0);
    float gettemp();
    void settemp(float x);
    void startTime();
    void endTime();
    bool getTimeState();
    void settempNature(float x);
    static const float avgTemp=25.0;
private:
    float temperature;
    QTimer* times;
    bool nature;
public slots:

signals:
    void timeover();
private slots:
    void sig();
};

#endif // ROOM_H
