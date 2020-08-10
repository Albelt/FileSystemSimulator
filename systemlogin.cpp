#include "systemlogin.h"
#include "ui_systemlogin.h"
#include "systemmain.h"
#include <QIcon>
#include <QPushButton>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include "systemmain.h"

SystemLogin::SystemLogin(SystemMain *win_main, Super *super, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemLogin)
{
    ui->setupUi(this);

    this->win_main = win_main;
    this->super = super;

    this->setWindowTitle("文件系统登录");
    this->setWindowIcon(QIcon(":/icon/icons/login.png"));
    this->setFixedSize(480, 340);

    connect(ui->btn_cancle, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->btn_login, &QPushButton::clicked, this, [=](){
        QString password = ui->lineEdit->text();
        emit this->CheckPassword(password);
    });
}


SystemLogin::~SystemLogin()
{
    delete ui;
}


void SystemLogin::CheckPassword(QString pwd)
{
    std::string pwd_std = pwd.toStdString();
    if(pwd_std == this->super->password){
        this->win_main->show();
        this->close();
    }
    else{
        QMessageBox::critical(this, "错误", "密码错误，登录失败！" );
    }
}
