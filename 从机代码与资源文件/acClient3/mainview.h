#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include <mainmodel.h>
#include <QTimer>
#include <QCloseEvent>
namespace Ui {
class mainView;
}

class mainView : public QWidget
{
    Q_OBJECT

public:
    explicit mainView(QWidget *parent = 0);
    ~mainView();

    void setState(int workMode, float lowTemp, float highTemp);
private slots:

    void sendworkTemp();
    void freshtoomtemp(float x);
    void showcost(QString a,QString b);
    void changewind();
    void changewid();
    void changestate(int x);
    void on_windspeed_clicked();

    void on_pushButton_up_clicked();

    void on_pushButton_down_clicked();

    void on_pushButton_off_clicked();

private:
    Ui::mainView *ui;
    mainmodel * x;
    bool clickMux;
    QTimer* timetochange;
    int wind;
protected:
     void closeEvent(QCloseEvent *event);
signals:
     void clos();
};

#endif // MAINVIEW_H
