#include "dialogopenfile.h"
#include "ui_dialogopenfile.h"
#include <QCloseEvent>


DialogOpenFile::DialogOpenFile(std::string file_name, std::string file_path, int index_block, int pid, File *file, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOpenFile)
{
    ui->setupUi(this);
    this->setWindowTitle("打开文件");
    this->setWindowIcon(QIcon(":/icon/icons/open.png"));
    this->setAttribute(Qt::WA_DeleteOnClose);

    this->file_name = file_name;
    this->file_path = file_path;
    this->index_block = index_block;
    this->pid = pid;
    this->file = file;

    this->file->access_count ++;

    connect(ui->btn_close, &QPushButton::clicked, this, [=](){
        this->close();
    });


    /*display info*/
    std::string temp;
    temp.append("文件名         " + this->file_name + "\n");
    temp.append("文件路径       " + this->file_path + "\n");
    temp.append("索引节点       " + std::to_string(this->index_block) + "\n");
    temp.append("虚拟进程ID     " + std::to_string(this->pid) + "\n");

    QString info = QString::fromStdString(temp);
    ui->text->setPlainText(info);
}

DialogOpenFile::~DialogOpenFile()
{
    delete ui;
}

void DialogOpenFile::closeEvent(QCloseEvent *event)
{
    this->file->access_count --;
    emit closeWindowSignal();
}
