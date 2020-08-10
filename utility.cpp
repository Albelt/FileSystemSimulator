#include "utility.h"

Utility::Utility()
{

}

std::vector<std::string> Utility::split(const std::string &str, const char pattern)
{
    std::vector<std::string> result;
    std::stringstream input(str);
    std::string temp;
    while(std::getline(input, temp, pattern))
        result.push_back(temp);
    return result;
}

std::string Utility::removeLastItem(const std::string &str, const char parttern)
{
    std::vector<std::string> items = Utility::split(str, parttern);
    std::string result;
    for(unsigned i=0; i<items.size()-1; i++)
    {
        result.append(items.at(i));
        if(i != items.size()-2)
            result.push_back(parttern);
    }

    return result;
}

void Utility::refreshUserComboBox(QComboBox *box, std::vector<User> *user_list)
{
    box->clear();
    for(unsigned i=0; i<user_list->size(); i++)
    {
        box->addItem(QString::fromStdString(user_list->at(i).name));
    }
}

void Utility::refreshGroupComboBox(QComboBox *box, std::vector<Group> *group_list)
{
    box->clear();
    for(unsigned i=0; i<group_list->size(); i++)
    {
        box->addItem(QString::fromStdString(group_list->at(i).name));
    }
}


