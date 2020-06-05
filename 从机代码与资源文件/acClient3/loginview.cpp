#include "loginview.h"
#include "ui_loginview.h"

loginview::loginview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loginview)
{
    ui->setupUi(this);
    x=new loginModel();
    connect(x,SIGNAL(transtomainwin(int,float,float)),this,SLOT(tansfer(int,float,float)));
    connect(x,SIGNAL(msg(QString)),this,SLOT(recieveMsg(QString)));

}

loginview::~loginview()
{
    delete ui;
}

/**
 * @brief loginview::on_Button_login_clicked
 * 按下登录按钮
 */
void loginview::on_Button_login_clicked()
{
     x->login1(ui->lineEdit_roomnumber->text(),ui->lineEdit_id->text());
}

/**
 * @brief loginview::tansfer
 * 界面转换
 * @param workModel
 * 工作模式
 * @param lowTemp
 * 最低温度
 * @param highTemp
 * 最高温度
 */
void loginview::tansfer(int workModel, float lowTemp, float highTemp)
{
    a = new mainView();
    a->setState(workModel,lowTemp,highTemp);
    a->show();
    this->hide();
    connect(a,SIGNAL(clos()),this,SLOT(reconnect()));
}

/**
 * @brief loginview::reconnect
 * 监听子窗口的关闭信号
 */
void loginview::reconnect()
{
    this->show();
    delete x;
    delete a;
    x=new loginModel();
    connect(x,SIGNAL(transtomainwin(int,float,float)),this,SLOT(tansfer(int,float,float)));
}

void loginview::recieveMsg(QString x)
{
    ui->lineEdit_msg->setText(x);
}
