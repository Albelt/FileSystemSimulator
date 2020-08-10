/*用户层*/

#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "simulationdisk.h"
#include "group.h"

class User //普通用户
{
public:
    User(std::string name, std::string pwd, int user_dir_block);    //构造函数，已经存在的用户
    User(std::string name, std::string pwd, SimulationDisk *disk);  //构造函数，新创建的用户

    /*User类*/
    std::string              name;            //用户名
    std::string              password;        //用户密码
    int                      user_dir_block;  //用户目录所在block
    std::vector<std::string> *gname_list;     //用户加入的用户组的名字列表
};

class Super //超级用户
{
public:
    Super();

    /*Super类*/
    std::string         password;            //管理员密码
    bool                is_dir_initialized;  //用户目录是否初始化
    SimulationDisk      *disk;               //虚拟磁盘对象指针
    std::vector<User>   *user_list;          //普通用户列表
    std::vector<Group>  *group_list;         //用户组列表

    void InitUserDir(); //磁盘为空时，初始化用户目录
};

#endif // USER_H
