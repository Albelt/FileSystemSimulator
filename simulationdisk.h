/*模拟层*/

#ifndef DISK_H
#define DISK_H

#include <stack>
#include "physicalmedium.h"

typedef struct superblcok{
    unsigned int free_block_num;
    int next_group_head_idx;                    // = -1表示结束
    std::stack<unsigned int> blocks_idx_stack;
} SuperBlock;

class SimulationDisk
{
public:
    SimulationDisk(PhysicalMedium *medium);

public:
    PhysicalMedium   *medium;          //物理层物理媒介对象
    unsigned int     block_size;       //每个block有多少byte
    unsigned int     total_block_num;  //磁盘总block数
    unsigned int     remain_block_num; //空闲的block数
    unsigned int     total_byte_num;   //总共多少byte
    /*成组链法空闲块管理*/
    unsigned int     first_block_idx;         //第0组第0块号码
    unsigned int     max_block_per_group;     //每组最多block数
    unsigned int     max_group_num;           //最大组数

    /*block operations*/
    void*      getBlockAddress(unsigned int index);    //给定块号，返回块首地址
    bool       readOneBlock(unsigned int index, void* content, unsigned int size);       //读取整块的内容
    bool       writeOneBlock(unsigned int index, void* content, unsigned int offset, unsigned int size);  //将content中start开始，大小为size的部分写入块
    int        allocateOneBlock(); //return=-1代表分配失败
    void       freeOneBlock(unsigned int index);

    /*Disk init, load, save and free*/
    void    Initialization();         //成组链法初始化空磁盘
    void    LoadDisk();               //Load Disk from physical medium
    void    SaveDisk();               //Save Disk info and content

    /*super block load and save*/
    void    ReadSuperBlock();
    void    WriteSuperBlock();


private:
    void        *disk_head;        //虚拟磁盘在内存中的指针(内存块的头)
    SuperBlock  *super_block;      //super block

};

#endif // DISK_H
