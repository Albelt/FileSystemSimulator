#include "group.h"

Group::Group(std::string name, bool read, bool write, bool execute, std::vector<std::string> *uname_list)
{
    this->name = name;
    this->read = read;
    this->write = write;
    this->execute = execute;
    this->uname_list = uname_list;
}
