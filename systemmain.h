#ifndef SYSTEMMAIN_H
#define SYSTEMMAIN_H

#include <QMainWindow>
#include <vector>
#include <QCloseEvent>
/*交互层*/

#include "info.h"
#include "usermain.h"

namespace Ui {
class SystemMain;
}

class SystemMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit SystemMain(Info *info, Super *super, QWidget *parent = nullptr); //构造函数
    ~SystemMain();

    void RefreshGroupInfo();    //刷新用户组信息

public slots:
    void CheckNamePassword(std::string name, std::string password); //检查普通用户的用户名和密码
    void CreateUser(std::string name, std::string password);    //创建新的普通用户

private:
    /*SystemMain类*/
    Ui::SystemMain *ui;
    Info *info;     //物理层用户、用户组信息对象
    Super *super;   //管理员对象

protected:
    void closeEvent(QCloseEvent *event); //主窗口关闭事件
};

#endif // SYSTEMMAIN_H
