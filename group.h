/*用户层*/

#ifndef GROUP_H
#define GROUP_H

#include <string>
#include <vector>

class Group
{
public:
    Group(std::string name, bool read, bool write, bool execute, std::vector<std::string> *uname_list); //构造函数

    /*Group类*/
    std::string                 name;       //用户组名
    bool                        read;       //读权限
    bool                        write;      //写权限
    bool                        execute;    //执行权限
    std::vector<std::string>    *uname_list;//用户组包含用户的用户名列表
};

#endif // GROUP_H
