#include "user.h"
#include "file.h"


User::User(std::string name, std::string pwd, int user_dir_block)
{
    this->name = name;
    this->password = pwd;
    this->user_dir_block = user_dir_block;
}

User::User(std::string name, std::string pwd, SimulationDisk *disk)
{
    this->name = name;
    this->password = pwd;
    File user_dir(name, name, true, disk);
    this->user_dir_block = user_dir.index_block;
    this->gname_list = new std::vector<std::string>;
}

Super::Super()
{

}

void Super::InitUserDir()
{
    for(unsigned i=0; i<this->user_list->size(); i++)
    {
        File user_dir(this->user_list->at(i).name, this->user_list->at(i).name, true, this->disk);
        this->user_list->at(i).user_dir_block = user_dir.index_block;
    }
    this->is_dir_initialized = true;
}


