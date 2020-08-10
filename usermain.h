/*交互层*/

#ifndef USERMAIN_H
#define USERMAIN_H

#include <QWidget>
#include <stack>
#include <QRandomGenerator>
#include "info.h"
#include "user.h"
#include "file.h"

namespace Ui {
class UserMain;
}

class UserMain : public QWidget
{
    Q_OBJECT

public:
    explicit UserMain(Super *super, std::string user_name, QWidget *parent = nullptr); //构造函数
    ~UserMain();

    /*UserMain类*/
    Super        *super;    //管理员对象
    User         *user;     //登录的用户对象
    std::string             current_dir_name; // looks like /user_name/dir1/dir11
    Dir                     *current_dir;   //当前路径的文件对象
    std::stack<unsigned>    stack_dir_idx;  //进入的目录的索引节点依次压栈，用于返回上级目录功能
    std::vector<File>       *current_dir_files; //当前路径下所有文件的对象列表

    void         RefreshDir();                          //刷新当前路径
    void         RefreshFileAttributes();               //刷新文件属性
    void         ReadCurrentDirFiles();                 //读取当前路径下所有文件
    void         WriteIndexBack();                      //将内存中索引内容写回磁盘索引节点：切换路径时、用户退出时
    bool         NameDuplicate(std::string file_name); //检查在当前路径下是否重名

protected:
    void closeEvent(QCloseEvent *event); //用户主窗口关闭事件

private:
    Ui::UserMain *ui;

};

#endif // USERMAIN_H
