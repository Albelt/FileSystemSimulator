#include "info.h"
#include "utility.h"
#include <cstdlib>
#include <QDebug>
#include <QDir>

Info::Info(Super *super, std::vector<User> *user_list, std::vector<Group> *group_list, const char *save_dir)
{
    this->super = super;
    this->user_list = user_list;
    this->group_list = group_list;
    this->save_dir = save_dir;
}


void Info::LoadInfo()
{
    char super_file[100], common_file[100], group_file[100];
    strcpy(super_file, this->save_dir);
    strcpy(common_file, this->save_dir);
    strcpy(group_file, this->save_dir);
    strcat(super_file, "super");
    strcat(common_file, "common");
    strcat(group_file, "group");

    std::fstream fs;
    char buffer[1024];
    std::string temp;
    std::vector<std::string> one_info;

    /*super user info*/
    fs.open(super_file, std::fstream::in);
    if(! fs.is_open()){
        qDebug() << "can't open super user file";
        exit(100);
    }
    fs.getline(buffer, 1024);
    temp = buffer;
    one_info = Utility::split(temp, ',');
    this->super->password = one_info.at(0);
    this->super->is_dir_initialized = std::stoi(one_info.at(1));
    fs.close();


    /*common user info*/
    fs.open(common_file, std::fstream::in);
    if(! fs.is_open()){
        qDebug() << "can't open common user file";
        exit(100);
    }
    while(! fs.eof())
    {
        fs.getline(buffer, 1024);
        if(buffer[0] == '\0')
            break;
        temp = buffer;
        one_info.clear();
        one_info = Utility::split(temp, ',');
        User user(one_info.at(0), one_info.at(1), std::stoi(one_info.at(2)));
        std::vector<std::string> *gname_list = new std::vector<std::string>;
        for(unsigned i=3; i<one_info.size(); i++)
        {
            gname_list->push_back(one_info.at(i));
        }
        user.gname_list = gname_list;
        user_list->push_back(user);
    }
    fs.close();


    /*group info*/
    fs.open(group_file, std::fstream::in);
    if(! fs.is_open()){
        qDebug() << "can't open group file";
        exit(100);
    }
    while(! fs.eof())
    {
        fs.getline(buffer, 1024);
        temp = buffer;
        if(buffer[0] == '\0')
            break;
        one_info.clear();
        one_info = Utility::split(temp, ',');
        std::vector<std::string> *uname_list = new std::vector<std::string>;
        for(unsigned int i=4; i<one_info.size(); i++)
        {
            uname_list->push_back(one_info.at(i));
        }
        Group group(one_info.at(0), std::stoi(one_info.at(1)), std::stoi(one_info.at(2)), std::stoi(one_info.at(3)), uname_list);
        this->group_list->push_back(group);
    }
    fs.close();
}

void Info::SaveInfo()
{
    char super_file[100], common_file[100], group_file[100];
    strcpy(super_file, this->save_dir);
    strcpy(common_file, this->save_dir);
    strcpy(group_file, this->save_dir);
    strcat(super_file, "super");
    strcat(common_file, "common");
    strcat(group_file, "group");

    std::fstream fs;

    /*super user info*/
    fs.open(super_file, std::fstream::out);
    if(! fs.is_open()){
        qDebug() << "can't open super user file";
        exit(100);
    }
    fs << this->super->password << "," << this->super->is_dir_initialized;
    fs.close();


    /*common user info*/
    fs.open(common_file, std::fstream::out);
    if(! fs.is_open()){
        qDebug() << "can't open common user file";
        exit(100);
    }
    for(unsigned int i=0; i<user_list->size(); i++)
    {
        fs << this->user_list->at(i).name << "," << this->user_list->at(i).password << "," << this->user_list->at(i).user_dir_block << ",";

        for(unsigned j=0; j<user_list->at(i).gname_list->size(); j++)
        {
            fs << user_list->at(i).gname_list->at(j) << ",";
        }

        if(i != user_list->size()-1)
            fs << "\n";
    }
    fs.close();


    /*group info*/
    fs.open(group_file, std::fstream::out);
    if(! fs.is_open()){
        qDebug() << "can't open group file";
        exit(100);
    }
    for(unsigned int i=0; i<group_list->size(); i++){
        fs << group_list->at(i).name << "," << group_list->at(i).read << "," << group_list->at(i).write << "," << group_list->at(i).execute << ",";
        for(unsigned j=0; j<group_list->at(i).uname_list->size(); j++)
            fs << group_list->at(i).uname_list->at(j) << ",";
        if(i != group_list->size()-1)
            fs << "\n";
    }
    fs.close();
}
