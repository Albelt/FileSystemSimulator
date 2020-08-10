#ifndef DIALOGOPENFILE_H
#define DIALOGOPENFILE_H

#include <QDialog>
#include <QString>
#include <string>
#include <file.h>

namespace Ui {
class DialogOpenFile;
}

class DialogOpenFile : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOpenFile(std::string file_name, std::string file_path, int index_block, int pid, File *file, QWidget *parent = nullptr); //构造函数
    ~DialogOpenFile();

private:
    /*DialogOpenFile类*/
    Ui::DialogOpenFile *ui;
    std::string     file_name;  //文件名
    std::string     file_path;  //文件路径
    int             index_block; //文件索引节点块号
    int             pid;         //打开文件的进程号
    File            *file;       //文件对象指针

protected:
    void closeEvent(QCloseEvent *event); //文件打开窗口关闭事件


signals:
    void            closeWindowSignal();
};

#endif // DIALOGOPENFILE_H
