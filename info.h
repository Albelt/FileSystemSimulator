/*物理层*/

#ifndef USERINFO_H
#define USERINFO_H

#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include "user.h"
#include "group.h"

class Info
{
public:
    Info(Super *super, std::vector<User> *user_list, std::vector<Group> *group_list, const char *save_dir); //构造函数

    Super               *super;         //管理员对象指针
    std::vector<User>   *user_list;     //普通用户对象列表
    std::vector<Group>  *group_list;    //用户组对象列表
    const char          *save_dir;      //信息保存目录

public:
    void LoadInfo();    //从配置文件中加载用户、用户组信息
    void SaveInfo();    //将用户、用户组信息保存到配置文件中
};

#endif // USERINFO_H
