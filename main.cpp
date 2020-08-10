#include "systemlogin.h"
#include "systemmain.h"
#include "info.h"
#include "physicalmedium.h"
#include "user.h"
#include "simulationdisk.h"
#include "file.h"

#include <vector>
#include <list>
#include <QDebug>
#include <QTime>
#include <QDate>

#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    /*用户层*/
    Super *super = new Super();
    std::vector<User> *user_list = new std::vector<User>;
    std::vector<Group> *group_list = new std::vector<Group>;

    /*加载物理层info文件，初始化用户层super,user_list,group_list*/
    Info *info = new Info(super, user_list, group_list, "info/");
    info->LoadInfo();

    /*加载物理层medium文件，初始化模拟层disk*/
    PhysicalMedium *medium = new PhysicalMedium("medium/");
    SimulationDisk *disk = new SimulationDisk(medium);

    /*super*/
    super->user_list = user_list;
    super->group_list = group_list;
    super->disk = disk;
    if(super->is_dir_initialized == false)
        super->InitUserDir();


    /*加载交互层*/
    SystemMain *win_main = new SystemMain(info, super);
    SystemLogin win_login(win_main, super);
    win_login.show();

    return a.exec();
}
