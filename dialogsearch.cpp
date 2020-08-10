#include "dialogsearch.h"
#include "ui_dialogsearch.h"
#include <QIcon>
#include <QMessageBox>
#include <QString>

DialogSearch::DialogSearch(Dir *dir, std::string dir_name, SimulationDisk *disk, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSearch)
{
    ui->setupUi(this);
    this->setWindowTitle("搜索文件");
    this->setWindowIcon(QIcon(":/icon/icons/search.png"));
    this->setAttribute(Qt::WA_DeleteOnClose);

    this->dir = dir;
    this->dir_name = dir_name;
    this->disk = disk;

    connect(ui->btn_search, &QPushButton::clicked, this, [=](){
        QString temp = ui->edit_filename->text();
        if(temp == "")
            return;
        std::string file_name = temp.toStdString();
        this->result_list = this->Search(this->dir, this->dir_name, file_name);

        if(this->result_list.empty() == true)
        {
            QMessageBox::warning(this, "警告", "没有搜索到结果!");
            return;
        }

        ui->list_result->clear();
        for(unsigned i=0; i<this->result_list.size(); i++)
        {
            ui->list_result->addItem(QString::fromStdString(this->result_list.at(i)));
        }

    });


}


std::vector<std::string> DialogSearch::Search(File *current_dir, std::string dir_name, std::string filename)
{
    std::vector<std::string> results;

    /*读取目录文件*/
    Dir dir_file(current_dir->index_block, this->disk);

    /*读取目录下的文件*/
    std::vector<File> files;
    for(unsigned i=0; i<dir_file.blocks_num; i++)
    {
        File file(dir_file.blocks_list->at(i), this->disk);
        files.push_back(file);
    }

    /*在文件列表中查找*/
    for(unsigned i=0; i<files.size(); i++)
    {
        if(files.at(i).is_dir == false) //非目录文件
        {
            if(files.at(i).name == filename)
            {
                std::string result = dir_name + "/" + filename;
                results.push_back(result);
            }
        }
        else //目录文件
        {
            std::vector<std::string> sub_results;
            sub_results = this->Search(&files.at(i), dir_name + "/" + files.at(i).name, filename);
            for(unsigned j=0; j<sub_results.size(); j++)
            {
                results.push_back(sub_results.at(j));
            }
        }
    }

    return results;
}


DialogSearch::~DialogSearch()
{
    delete ui;
}

