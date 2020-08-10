#ifndef DIALOGSEARCH_H
#define DIALOGSEARCH_H

#include <QDialog>
#include <string>
#include <vector>
#include "simulationdisk.h"
#include "file.h"

namespace Ui {
class DialogSearch;
}

class DialogSearch : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSearch(Dir *dir, std::string dir_name, SimulationDisk *disk, QWidget *parent = nullptr);//构造函数
    ~DialogSearch();

    /*DialogSearch类*/
    Dir                       *dir;     //当前路径的文件对象
    std::string               dir_name; //当前路径的名称
    SimulationDisk            *disk;    //模拟磁盘对象
    std::vector<std::string>  result_list;  //搜索结果列表


    std::vector<std::string>    Search(File *current_dir, std::string dir_name, std::string filename); //递归搜索函数

private:
    Ui::DialogSearch *ui;
};

#endif // DIALOGSEARCH_H
