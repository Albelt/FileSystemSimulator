 /*模拟层*/

#ifndef FILE_H
#define FILE_H

#include <string>
#include <vector>
#include "simulationdisk.h"


class File
{
public:
    File(unsigned int index_block, SimulationDisk *disk);//构造函数，提供索引节点，从磁盘中初始化文件
    File(std::string name, std::string owner, bool is_dir, SimulationDisk *disk); //构造函数，提供文件信息，创建新文件

public:
    /*File类*/
    /*文件属性*/
    std::string     name;   //文件名
    unsigned int    size;   //文件体所占byte大小
    std::string     owner;  //文件所有者名字
    bool            is_dir; //是否为目录文件
    std::string     create_time; //创建时间
    std::string     modify_time; //最后修改时间
    /*模拟层存储信息*/
    SimulationDisk          *disk;          //模拟层磁盘
    unsigned                index_block;    //索引节点所在块号
    std::vector<unsigned>   *blocks_list;   //文件体存储在哪些block里面
    unsigned                blocks_num;     //blocks_list中有多少block
    /*其他信息*/
    unsigned int            access_count;   //访问计数，仅在内存中有该属性

    void    ReadIndexBlock();   //读取文件索引节点
    void    WriteIndexBlock();  //写入文件索引节点
    bool    WriteContent(void *content, int bytes); //写入文件内容
    void    ReadContent(void *content); //读取文件内容
    void    FreeFileBlocks();   //从Disk中Free掉文件体和文件头所占用的block
};


class Dir: public File
{
public:
    using File::File;


};

#endif // FILE_H
