/*交互层*/

#ifndef SYSTEMLOGIN_H
#define SYSTEMLOGIN_H

#include <QWidget>
#include "systemmain.h"
#include "user.h"

namespace Ui {
class SystemLogin;
}

class SystemLogin : public QWidget
{
    Q_OBJECT

public:
    explicit SystemLogin(SystemMain *win_main, Super *super, QWidget *parent = nullptr); //构造函数
    ~SystemLogin();
    /*SystemLogin类*/
    SystemMain *win_main;   //系统主窗口

public slots:
    void CheckPassword(QString pwd); //检查管理员密码

private:
    Ui::SystemLogin *ui;
    Super *super;       //管理员对象指针
};

#endif // SYSTEMLOGIN_H
