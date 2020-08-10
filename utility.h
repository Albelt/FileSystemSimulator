/*其他*/

#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <QComboBox>
#include <QString>

#include <user.h>

class Utility
{
public:
    Utility();

    static std::vector<std::string> split(const std::string &str, const char pattern);
    static std::string              removeLastItem(const std::string &str, const char parttern);

    static void refreshUserComboBox(QComboBox *box, std::vector<User> *user_list);
    static void refreshGroupComboBox(QComboBox *box, std::vector<Group> *group_list);
};

#endif // UTILITY_H
