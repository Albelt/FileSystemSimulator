#include "systemmain.h"
#include "ui_systemmain.h"
#include "utility.h"
#include "file.h"
#include <QIcon>
#include <QTabBar>
#include <QDebug>
#include <QWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <string>
#include <vector>
#include <QFile>

SystemMain::SystemMain(Info *info, Super *super, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SystemMain)
{
    ui->setupUi(this);
    this->setWindowTitle("文件系统模拟器");
    this->setWindowIcon(QIcon(":/icon/icons/function.png"));
    ui->tabWidget->setTabIcon(0, QIcon(":/icon/icons/setting.png"));
    ui->tabWidget->setTabIcon(1, QIcon(":/icon/icons/user.png"));
    ui->tabWidget->setTabIcon(2, QIcon(":/icon/icons/group.png"));

    this->info = info;
    this->super = super;



    /*-----------------------------------------------------系统管理面板------------------------------------------------------*/
    connect(ui->btn_chpwd, &QPushButton::clicked, this, [=](){
        /*修改管理员密码*/
        QString pwd = QInputDialog::getText(this, "输入", "输入原密码", QLineEdit::Password);
        if(pwd == "")
            return;
        if(pwd.toStdString() == super->password)
        {
            QString pwd_new = QInputDialog::getText(this, "输入", "输入新密码(英文和数字)", QLineEdit::Password);
            if(pwd_new == "")
                return;
            super->password = pwd_new.toStdString();
            QMessageBox::information(this, "成功", "管理员密码修改成功!");
        }
        else
        {
            QMessageBox::critical(this, "错误", "原密码输入错误!");
        }
    });

    connect(ui->btn_shutdown, &QPushButton::clicked, this, [=](){
        /*关闭系统*/
        QMessageBox::StandardButton is_yes = QMessageBox::question(this, "确认", "您确认要关闭系统吗?");
        if(is_yes == QMessageBox::Yes)
            this->close();
    });

    connect(ui->btn_format, &QPushButton::clicked, this, [=](){
        /*磁盘格式化*/
        QString pwd = QInputDialog::getText(this, "输入", "输入管理员密码", QLineEdit::Password);
        if(pwd == "")
            return;
        if(pwd.toStdString() == super->password)
        {
             //super
            this->super->is_dir_initialized = false;
            //disk_info
            this->super->disk->remain_block_num = this->super->disk->total_block_num;
            this->super->disk->first_block_idx = 0;
            this->super->disk->medium->is_empty = true;
            //common
            for(unsigned i=0; i<this->super->user_list->size(); i++)
                this->super->user_list->at(i).user_dir_block = -1;

            QMessageBox::information(this, "成功", "磁盘格式化成功!\n请重启系统!");
        }
        else
        {
            QMessageBox::critical(this, "错误", "管理员密码输入错误!");
        }
    });

    connect(ui->btn_about, &QPushButton::clicked, this, [=](){
        /*显示程序信息*/
        std::string display_info;
        display_info.append("磁盘块大小：" + std::to_string(this->super->disk->medium->block_size) + " byte\n");
        display_info.append("磁盘总空间：" + std::to_string(this->super->disk->medium->total_block_num * this->super->disk->medium->block_size / 1048576) + " MB\n");
        display_info.append("磁盘剩余空间：" + std::to_string(this->super->disk->medium->remain_block_num * this->super->disk->medium->block_size / 1048576) + " MB\n");
        display_info.append("开发者：文诗波、方程程、次央\n联系方式：albeltbo@foxmail.com\n");
        QString display_info_qt = QString::fromStdString(display_info);

        QMessageBox::about(this, "关于", display_info_qt);
    });




    /*------------------------------------------------------用户管理面板------------------------------------------------------------*/
    Utility::refreshUserComboBox(ui->cbox_user, this->super->user_list);

    connect(ui->btn_user_login, &QPushButton::clicked, this, [=](){
        /*用户登录*/
        std::string name = ui->edit_username->text().toStdString();
        std::string pwd = ui->edit_password->text().toStdString();
        emit CheckNamePassword(name, pwd);
    });

    connect(ui->btn_create_user, &QPushButton::clicked, this, [=](){
        /*创建用户*/
        std::string name = ui->edit_username->text().toStdString();
        std::string pwd = ui->edit_password->text().toStdString();
        emit CreateUser(name, pwd);
        Utility::refreshUserComboBox(ui->cbox_user, this->super->user_list);
        Utility::refreshUserComboBox(ui->cbox_user2, this->super->user_list);
    });

    connect(ui->btn_del_user, &QPushButton::clicked, this, [=](){
        /*删除用户*/
        QString pwd =  QInputDialog::getText(this, "输入", "输入管理员密码", QLineEdit::Password);
        if(pwd == "")
            return;
        if(pwd.toStdString() == this->super->password)
        {
            int currentIndex = ui->cbox_user->currentIndex();
            User *user = & this->super->user_list->at(currentIndex);
            /*从磁盘中递归删除用户目录*/
            File user_dir(user->user_dir_block, this->super->disk);
            user_dir.FreeFileBlocks();
            /*从用户组中删除用户*/
            for(unsigned i=0; i<this->super->group_list->size(); i++)
            {
                Group *group = & this->super->group_list->at(i);
                for(unsigned j=0; j<group->uname_list->size(); j++)
                {
                    if(group->uname_list->at(j) == user->name)
                        group->uname_list->erase(group->uname_list->begin() + j);
                }
            }
            /*从用户列表中删除用户*/
            this->super->user_list->erase(this->super->user_list->begin() + currentIndex);

            QMessageBox::information(this, "成功", "删除成功!");
        }
        else
        {
            QMessageBox::critical(this, "错误", "管理员密码输入错误!");
            return;
        }

        Utility::refreshUserComboBox(ui->cbox_user, this->super->user_list);
        Utility::refreshUserComboBox(ui->cbox_user2, this->super->user_list);
        this->RefreshGroupInfo();
    });

    connect(ui->btn_user_pwd, &QPushButton::clicked, this, [=](){
       QString pwd_origin =  QInputDialog::getText(this, "输入", "输入用户原来密码", QLineEdit::Password);
       if(pwd_origin == "")
           return;
       QString pwd_new = QInputDialog::getText(this, "输入", "输入用户新密码", QLineEdit::Password);
       if(pwd_new == "")
           return;

       int currentIndex = ui->cbox_user->currentIndex();
       User *user = & this->super->user_list->at(currentIndex);
       if(pwd_origin.toStdString() == user->password)
       {
           user->password = pwd_new.toStdString();
           QMessageBox::information(this, "成功", "用户密码修改成功!");
       }
       else
       {
           QMessageBox::critical(this, "错误", "原密码输入错误!");
       }
    });


    /*----------------------------------------------------------用户组管理面板----------------------------------------------------*/
    Utility::refreshUserComboBox(ui->cbox_user2, this->super->user_list);
    Utility::refreshGroupComboBox(ui->cbox_group, this->super->group_list);
    this->RefreshGroupInfo();

    connect(ui->cbox_group, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int currentIndex){
        /*显示选中用户组的信息*/
        if(currentIndex < 0)
            return;
        std::string group_info;
        Group *group = & this->super->group_list->at(currentIndex);
        group_info.append("组名          " + group->name + "\n");
        group_info.append("读权限        " + std::to_string(group->read) + "\n");
        group_info.append("写权限        " + std::to_string(group->write) + "\n");
        group_info.append("执行权限      " + std::to_string(group->execute) + "\n");
        group_info.append("包含用户\n");
        for(unsigned i=0; i<group->uname_list->size(); i++)
            group_info.append(group->uname_list->at(i) + "\n");
        ui->text_group_info->setPlainText(QString::fromStdString(group_info));
    });

    connect(ui->btn_add_group, &QPushButton::clicked, this, [=](){
        /*新建用户组*/
        QString group_name = QInputDialog::getText(this, "输入", "输入用户组名");
        if(group_name == "")
            return;
        for(unsigned i=0; i<this->super->group_list->size(); i++)
        {
            if(this->super->group_list->at(i).name == group_name.toStdString())
            {
                QMessageBox::critical(this, "错误", "该用户组已经存在!");
                return;
            }
        }
        std::vector<std::string> *names = new std::vector<std::string>;
        Group new_group(group_name.toStdString(), 0, 0, 0, names);
        this->super->group_list->push_back(new_group);
        QMessageBox::information(this, "成功", "用户组创建成功");
        Utility::refreshGroupComboBox(ui->cbox_group, this->super->group_list);
    });


    connect(ui->btn_modify_permission, &QPushButton::clicked, this, [=](){
        /*修改用户组权限*/
        int currentIndex = ui->cbox_group->currentIndex();
        if(currentIndex < 0)
            return;
        Group *group = & this->super->group_list->at(currentIndex);
        group->read = ui->checkBoxRead->isChecked();
        group->write = ui->checkBoxWrite->isChecked();
        group->execute = ui->checkBoxExecute->isChecked();
        QMessageBox::information(this, "成功", "用户组权限修改成功!");
        this->RefreshGroupInfo();
    });

    connect(ui->btn_add_to_group, &QPushButton::clicked, this, [=](){
        /*将用户添加到用户组*/
        int groupIndex = ui->cbox_group->currentIndex();
        int userIndex = ui->cbox_user2->currentIndex();
        if(groupIndex < 0 || userIndex < 0)
            return;
        User *user = & this->super->user_list->at(userIndex);
        Group *group = & this->super->group_list->at(groupIndex);
        for(unsigned i=0; i<group->uname_list->size(); i++)
        {
            if(group->uname_list->at(i) == user->name)
            {
                QMessageBox::warning(this, "警告", "该用户已经在用户组中了!");
                return;
            }
        }
        group->uname_list->push_back(user->name);
        user->gname_list->push_back(group->name);
        QMessageBox::information(this, "成功", "成功将用户添加到用户组!");
        this->RefreshGroupInfo();
    });

    connect(ui->btn_del_from_group, &QPushButton::clicked, this, [=](){
        /*将用户从用户组中删除*/
        int groupIndex = ui->cbox_group->currentIndex();
        int userIndex = ui->cbox_user2->currentIndex();
        if(groupIndex < 0 || userIndex < 0)
            return;
        User *user = & this->super->user_list->at(userIndex);
        Group *group = & this->super->group_list->at(groupIndex);
        int user_in_group_index = -1;
        for(unsigned i=0; i<group->uname_list->size(); i++)
        {
            if(group->uname_list->at(i) == user->name)
                user_in_group_index = i;
        }
        if(user_in_group_index == -1)
        {
            QMessageBox::warning(this, "警告", "该用户不在用户组中!");
            return;
        }
        group->uname_list->erase(group->uname_list->begin() + user_in_group_index);
        for(unsigned i=0; i<user->gname_list->size(); i++)
        {
            if(user->gname_list->at(i) == group->name)
            {
                user->gname_list->erase(user->gname_list->begin() + i);
                break;
            }
        }
        QMessageBox::information(this, "成功", "成功将用户从用户组删除!");
        this->RefreshGroupInfo();
    });


    connect(ui->btn_del_group, &QPushButton::clicked, this, [=](){
        /*删除用户组*/
        int groupIndex = ui->cbox_group->currentIndex();
        if(groupIndex < 0)
            return;
        if(QMessageBox::question(this, "确认", "是否删除用户组?") != QMessageBox::Yes)
            return;
        Group *group = & this->super->group_list->at(groupIndex);
        /*从用户信息中删除用户组*/
        for(unsigned i=0; i<this->super->user_list->size(); i++)
        {
            User *user = & this->super->user_list->at(i);
            for(unsigned j=0; j<user->gname_list->size(); j++)
            {
                if(user->gname_list->at(j) == group->name)
                {
                    user->gname_list->erase(user->gname_list->begin() + j);
                }
            }
        }
        /*从用户组列表中删除用户组*/
        this->super->group_list->erase(this->super->group_list->begin() + groupIndex);
        QMessageBox::information(this, "成功", "成功删除用户组!");
        this->RefreshGroupInfo();
        Utility::refreshGroupComboBox(ui->cbox_group, this->super->group_list);
    });
}



























SystemMain::~SystemMain()
{
    delete ui;
}

void SystemMain::RefreshGroupInfo()
{
    int currentIndex = ui->cbox_group->currentIndex();
    if(currentIndex < 0)
        return;
    if(this->super->group_list->size() == 0)
    {
        ui->text_group_info->clear();
        return;
    }
    std::string group_info;
    Group *group = & this->super->group_list->at(currentIndex);
    group_info.append("组名          " + group->name + "\n");
    group_info.append("读权限        " + std::to_string(group->read) + "\n");
    group_info.append("写权限        " + std::to_string(group->write) + "\n");
    group_info.append("执行权限      " + std::to_string(group->execute) + "\n");
    group_info.append("包含用户\n");
    for(unsigned i=0; i<group->uname_list->size(); i++)
        group_info.append(group->uname_list->at(i) + "\n");

    ui->text_group_info->setPlainText(QString::fromStdString(group_info));
}


void SystemMain::CheckNamePassword(std::string name, std::string password)
{
    /*check user name*/
    int user_idx = -1;
    for(unsigned int i=0; i<this->super->user_list->size(); i++)
    {
        if(this->super->user_list->at(i).name == name){
            user_idx = i;
            break;
        }
    }

    if(user_idx == -1){// user doesn't exist
        QMessageBox::critical(this, "错误", "该用户不存在!");
    }
    else{// check password
        if(this->super->user_list->at(user_idx).password == password)
        {
            UserMain *user = new UserMain(this->super, name);
            user->show();
        }
        else
        {
            QMessageBox::critical(this, "错误", "用户密码输入错误!");
        }
    }

}


void SystemMain::CreateUser(std::string name, std::string password)
{
    /*check user name*/
    for (unsigned int i=0; i<this->super->user_list->size(); i++) {
        if(this->super->user_list->at(i).name == name)
        {
            QMessageBox::critical(this, "错误", "用户名已存在!");
            return;
        }
    }

    /*create new user*/
    User user(name, password, this->super->disk); //创建用户并初始化用户目录
    this->super->user_list->push_back(user);
    QMessageBox::information(this, "成功", "成功创建用户!");
}


void SystemMain::closeEvent(QCloseEvent *event)
{
    qDebug() << "system main window closed";
    qDebug() << event;

    super->disk->SaveDisk(); //保存磁盘属性、磁盘内容
    info->SaveInfo();        //保存用户、用户组信息
    if(super->disk->medium->is_empty == true) //格式化后，删除磁盘文件以节省空间
    {
        char disk_file[100];
        strcpy(disk_file, this->super->disk->medium->save_dir);
        strcat(disk_file, "disk_content");
        QFile temp(QString::fromStdString(disk_file));
        if(temp.exists() == true)
            temp.remove();
    }
    qDebug() << "disk and info saved";
}
