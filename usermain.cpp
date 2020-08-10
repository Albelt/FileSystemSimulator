#include "usermain.h"
#include "ui_usermain.h"
#include "dialogsearch.h"
#include "dialogopenfile.h"
#include <QIcon>
#include <QListWidget>
#include <QDebug>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <utility.h>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QDate>
#include <QTime>
#include <QRandomGenerator>

UserMain::UserMain(Super *super, std::string user_name,  QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserMain)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromStdString(user_name));
    this->setWindowIcon(QIcon(":/icon/icons/user.png"));
    this->setAttribute(Qt::WA_DeleteOnClose);


    this->super = super;
    for(unsigned i=0; i<super->user_list->size(); i++)
    {
        if(super->user_list->at(i).name == user_name)
        {
            this->user = & super->user_list->at(i);
            break;
        }
    }

    this->current_dir_name = "/" + this->user->name;
    ui->label_current_dir->setText(QString::fromStdString(this->current_dir_name));
    this->current_dir = new Dir(this->user->user_dir_block, super->disk);
    this->current_dir_files = new std::vector<File>;
    this->ReadCurrentDirFiles();
    this->RefreshDir();


    /*各个组件的功能实现*/
    connect(ui->btn_create_file, &QPushButton::clicked, this, [=](){
        /*在当前目录下创建文件*/
        QString file_name = QInputDialog::getText(this, "输入", "输入文件名");
        if(file_name == "")
            return;
        if(this->NameDuplicate(file_name.toStdString()))
        {
            QMessageBox::critical(this, "错误", "一个文件夹中不能有相同的文件名!");
            return;
        }
        File file(file_name.toStdString(), this->user->name, false, this->super->disk);
        this->current_dir->blocks_list->push_back(file.index_block);
        this->current_dir->blocks_num ++;
        this->current_dir_files->push_back(file);
        this->RefreshDir();
    });

    connect(ui->btn_create_dir, &QPushButton::clicked, this, [=](){
        /*在当前目录下创建目录*/
        QString file_name = QInputDialog::getText(this, "输入", "输入文件夹名");
        if(file_name == "")
            return;
        if(this->NameDuplicate(file_name.toStdString()))
        {
            QMessageBox::critical(this, "错误", "一个文件夹中不能有相同的文件名!");
            return;
        }
        Dir dir(file_name.toStdString(), this->user->name, true, this->super->disk);
        this->current_dir->blocks_list->push_back(dir.index_block);
        this->current_dir->blocks_num ++;
        this->current_dir_files->push_back(dir);
        this->RefreshDir();
    });

    connect(ui->btn_delete, &QPushButton::clicked, this, [=](){
        /*删除选中的文件或文件夹*/
        int row = ui->list_files->currentRow();
        if(row < 0)
            return;
        this->current_dir_files->at(row).FreeFileBlocks();
        this->current_dir_files->erase(this->current_dir_files->begin() + row);
        this->current_dir->blocks_list->erase(this->current_dir->blocks_list->begin() + row);
        this->current_dir->blocks_num --;
        this->RefreshDir();
        this->RefreshFileAttributes();
    });

    connect(ui->list_files, QOverload<int>::of(&QListWidget::currentRowChanged), this, [=](int currentRow){
        /*显示选中文件的属性*/
        if(currentRow < 0)
            return;
        std::string attributes;
        File file = this->current_dir_files->at(currentRow);
        attributes.append("名字                         " + file.name + "\n");
        attributes.append("大小                         " + std::to_string(file.size) + " bytes \n");
        attributes.append("所有者                     " + file.owner + "\n");
        attributes.append("创建时间                  " + file.create_time + "\n");
        attributes.append("最后修改时间           " + file.modify_time + "\n");
        attributes.append("进程访问计数           " + std::to_string(file.access_count) + "\n");
        attributes.append("索引节点块号           " + std::to_string(file.index_block) + "\n");
        if(file.is_dir == true)
        {
            attributes.append( "包含文件(夹)数量      " + std::to_string(file.blocks_num) + "\n");
        }
        else
        {
            attributes.append("文件体占用磁盘块    " + std::to_string(file.blocks_num) + "\n");
        }

        ui->text_attributes->setPlainText(QString::fromStdString(attributes));
    });


    connect(ui->btn_rename, &QPushButton::clicked, this, [=](){
        /*文件或文件夹重命名*/
        QString file_name = QInputDialog::getText(this, "输入", "输入新文件(夹)名");
        if(file_name == "")
            return;
        if(this->NameDuplicate(file_name.toStdString()))
        {
            QMessageBox::critical(this, "错误", "一个文件夹中不能有相同的文件名!");
            return;
        }
        int row = ui->list_files->currentRow();
        if(row < 0)
            return;
        this->current_dir_files->at(row).name = file_name.toStdString();
        this->RefreshFileAttributes();
        this->RefreshDir();
    });


    connect(ui->btn_open_dir, &QPushButton::clicked, this, [=](){
        /*进入选择目录*/
        int row = ui->list_files->currentRow();
        if(row < 0)
            return;
        File file = this->current_dir_files->at(row);
        if(file.is_dir == false)
        {
            QMessageBox::information(this, "警告", "当前选择的不是目录!");
            return;
        }

        /*父目录处理*/
        this->stack_dir_idx.push(this->current_dir->index_block); //父目录压栈
        this->WriteIndexBack(); //将父目录在内存中的更改写入Disk
        delete this->current_dir;
        delete this->current_dir_files;
        /*切换到子目录*/
        this->current_dir = new Dir(file.index_block, this->super->disk);
        this->current_dir_name.append("/" + this->current_dir->name);
        ui->label_current_dir->setText(QString::fromStdString(this->current_dir_name));
        this->current_dir_files = new std::vector<File>;
        this->ReadCurrentDirFiles();
        this->RefreshDir();
    });


    connect(ui->btn_return_pre, &QPushButton::clicked, this, [=](){
        /*返回上级目录*/
        if(this->stack_dir_idx.empty() == true)
        {
            QMessageBox::information(this, "警告", "已经在主目录了!");
            return ;
        }

        /*子目录处理*/
        this->WriteIndexBack();
        delete this->current_dir;
        delete this->current_dir_files;
        /*返回到上级父目录*/
        unsigned index_block = this->stack_dir_idx.top(); //上级父目录出栈
        this->stack_dir_idx.pop();
        this->current_dir = new Dir(index_block, this->super->disk);
        this->current_dir_name = Utility::removeLastItem(this->current_dir_name, '/');
        ui->label_current_dir->setText(QString::fromStdString(this->current_dir_name));
        this->current_dir_files = new std::vector<File>;
        this->ReadCurrentDirFiles();
        this->RefreshDir();
    });


    connect(ui->btn_return_home, &QPushButton::clicked, this, [=](){
        /*返回主目录*/
        if(this->stack_dir_idx.empty() == true)
        {
            QMessageBox::information(this, "警告", "已经在主目录了!");
            return ;
        }

        /*子目录处理*/
        this->WriteIndexBack();
        delete this->current_dir;
        delete this->current_dir_files;
        /*返回到用户主目录*/
        while(this->stack_dir_idx.empty() == false) //清空栈
        {
            this->stack_dir_idx.pop();
        }
        unsigned index_block = this->user->user_dir_block;
        this->current_dir = new Dir(index_block, this->super->disk);
        this->current_dir_name = "/" + this->user->name;
        ui->label_current_dir->setText(QString::fromStdString(this->current_dir_name));
        this->current_dir_files = new std::vector<File>;
        this->ReadCurrentDirFiles();
        this->RefreshDir();
    });


    connect(ui->btn_search, &QPushButton::clicked, this, [=](){
        /*在当前目录下递归搜索*/
        DialogSearch *search_win = new DialogSearch(this->current_dir, this->current_dir_name, this->super->disk, this);
        search_win->show();
    });


    connect(ui->btn_read_file, &QPushButton::clicked, this, [=](){
        /*将文件从Disk中读取出来*/
        int currentRow = ui->list_files->currentRow();
        if(currentRow < 0)
            return;
        File *file = &this->current_dir_files->at(currentRow);
        if(file->is_dir == true)
        {
            QMessageBox::critical(this, "错误", "目录无法读取!");
            return;
        }
        if(file->size == 0)
        {
            QMessageBox::critical(this, "错误", "空文件无法读取");
            return;
        }
        QString file_name = QFileDialog::getSaveFileName(this, "选择读出源");
        if(file_name == "")
            return;
        QFile f(file_name);
        if(f.open(QIODevice::WriteOnly) == false)
        {
            qDebug() << "can't open file";
            exit(0);
        }
        void *content = malloc(file->size);
        if(content == NULL)
        {
            qDebug() << "memory allocate failed";
            exit(0);
        }
        file->ReadContent(content);
        f.write((char*)content, file->size);
        f.close();
        free(content);
    });


    connect(ui->btn_write_file, &QPushButton::clicked, this, [=](){
        /*将文件写入Disk的文件中*/
        int currentRow = ui->list_files->currentRow();
        if(currentRow < 0)
            return;
        File *file = &this->current_dir_files->at(currentRow);
        if(file->is_dir == true)
        {
            QMessageBox::critical(this, "错误", "目录无法写入!");
            return;
        }
        if(file->size != 0)
        {
            QMessageBox::critical(this, "错误", "非空文件无法写入!");
            return;
        }
        QString file_name = QFileDialog::getOpenFileName(this, "选择写入源");
        if(file_name == "")
            return;
        QFileInfo file_info(file_name);
        int file_byte_num = 0;
        if(file_info.exists())
            file_byte_num = file_info.size();
        if(file_byte_num == 0)
        {
            QMessageBox::information(this, "提示", "源文件为空!");
            return;
        }
        QFile f(file_name);
        if(f.open(QIODevice::ReadOnly) == false)
        {
            qDebug() << "can't open source file";
            exit(0);
        }
        void *content = malloc(file_byte_num);
        if(content == NULL)
        {
            qDebug() << "memory allocate failed";
            exit(0);
        }
        if(f.read((char*)content, file_byte_num) != file_byte_num)
        {
            qDebug() << "read source file failed";
            exit(0);
        }
        file->WriteContent(content, file_byte_num);
        file->modify_time = QDate::currentDate().toString("yyyy-MM-dd").toStdString() + " " + QTime::currentTime().toString("hh:mm:ss").toStdString();
        f.close();
        free(content);
        this->RefreshFileAttributes();
    });


    connect(ui->btn_open_file, &QPushButton::clicked, this, [=](){
        /*模拟文件打开和关闭的行为*/
        int currentRow = ui->list_files->currentRow();
        if(currentRow < 0)
            return;
        File *file = & this->current_dir_files->at(currentRow);
        int pid = QRandomGenerator::global()->bounded(0, 65536);
        DialogOpenFile *open_file_win = new DialogOpenFile(file->name, this->current_dir_name, file->index_block, pid, file, this);
        open_file_win->show();
        this->RefreshFileAttributes();

        /*捕获该窗口关闭事件*/
        connect(open_file_win, &DialogOpenFile::closeWindowSignal, this, [=](){
            this->RefreshFileAttributes();
        });
    });


}































UserMain::~UserMain()
{
    delete ui;
    delete current_dir;
    delete current_dir_files;
}

void UserMain::RefreshFileAttributes()
{
    /*显示选中文件的属性*/
    int currentRow = ui->list_files->currentRow();
    if(currentRow < 0)
    {
        ui->text_attributes->clear();
        return;
    }
    if(this->current_dir_files->size() == 0)
    {
        ui->text_attributes->clear();
        return;
    }
    std::string attributes;
    File file = this->current_dir_files->at(currentRow);
    attributes.append("名字                         " + file.name + "\n");
    attributes.append("大小                         " + std::to_string(file.size) + " bytes \n");
    attributes.append("所有者                     " + file.owner + "\n");
    attributes.append("创建时间                  " + file.create_time + "\n");
    attributes.append("最后修改时间           " + file.modify_time + "\n");
    attributes.append("进程访问计数           " + std::to_string(file.access_count) + "\n");
    attributes.append("索引节点块号           " + std::to_string(file.index_block) + "\n");
    if(file.is_dir == true)
    {
        attributes.append( "包含文件(夹)数量      " + std::to_string(file.blocks_num) + "\n");
    }
    else
    {
        attributes.append("文件体占用磁盘块    " + std::to_string(file.blocks_num) + "\n");
    }

    ui->text_attributes->setPlainText(QString::fromStdString(attributes));
}

void UserMain::RefreshDir()
{
    QListWidget *list = ui->list_files;
    list->clear();
    for(unsigned i=0; i<this->current_dir_files->size(); i++)
    {
        QListWidgetItem *item;
        if(this->current_dir_files->at(i).is_dir == true)
        {
            item = new QListWidgetItem(QIcon(":/icon/icons/folder.png"), QString::fromStdString(this->current_dir_files->at(i).name), list);
        }
        else
        {
            item = new QListWidgetItem(QIcon(":/icon/icons/file.png"), QString::fromStdString(this->current_dir_files->at(i).name), list);
        }
        list->addItem(item);
    }
}

void UserMain::ReadCurrentDirFiles()
{
    for(unsigned i=0; i<this->current_dir->blocks_list->size(); i++)
    {
        File file(this->current_dir->blocks_list->at(i), this->super->disk);
        this->current_dir_files->push_back(file);
    }
}

void UserMain::WriteIndexBack()
{
    this->current_dir->WriteIndexBlock();
    for(unsigned i=0; i<this->current_dir_files->size(); i++)
    {
        this->current_dir_files->at(i).WriteIndexBlock();
    }
}

bool UserMain::NameDuplicate(std::string file_name)
{
    for(unsigned i=0; i<this->current_dir_files->size(); i++)
    {
        if(current_dir_files->at(i).name == file_name)
            return true;
    }
    return false;
}

void UserMain::closeEvent(QCloseEvent *event)
{
    this->WriteIndexBack();
    qDebug() << "write index back";
}
