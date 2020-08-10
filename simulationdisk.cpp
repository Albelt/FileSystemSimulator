#include "simulationdisk.h"
#include "cstdlib"
#include <cstring>
#include <QDebug>

SimulationDisk::SimulationDisk(PhysicalMedium *medium)
{
    this->medium = medium;
    this->medium->LoadDiskInfo();

    this->block_size = this->medium->block_size;
    this->total_block_num = this->medium->total_block_num;
    this->remain_block_num = this->medium->remain_block_num;
    this->total_byte_num = this->total_block_num * this->block_size;

    this->first_block_idx = this->medium->first_block_idx;
    this->max_block_per_group = 1024;
    this->max_group_num = this->total_block_num / this->max_block_per_group;

    /*申请内存空间并加载虚拟磁盘*/
    this->disk_head = malloc(this->total_byte_num);
    if(this->disk_head == NULL){
        qDebug() << "memory allocating failed";
        exit(100);
    }
    this->LoadDisk();

    /*将成组链第0组拷贝到内存超级快中*/
    this->super_block = new SuperBlock;
    this->ReadSuperBlock();

}

void* SimulationDisk::getBlockAddress(unsigned int index)
{
    if(index >= this->total_block_num)
        return NULL;
    return ((char*)this->disk_head + index * this->block_size);
}

bool SimulationDisk::readOneBlock(unsigned int index, void* content, unsigned int size)
{
    void *block_address = this->getBlockAddress(index);
    if(block_address == NULL)   return false;
    memcpy(content, block_address, size);
    return true;
}

bool SimulationDisk::writeOneBlock(unsigned int index, void *content, unsigned int offset, unsigned int size)
{
    if(size > this->block_size) return false;

    void* block_address = this->getBlockAddress(index);
    if(block_address == NULL)   return false;

    void *copy_start = (char*)content + offset;
    memcpy(block_address, copy_start, size);

    return true;
}


void SimulationDisk::Initialization()
{
    for(unsigned int i=0; i<this->max_group_num; i++)
    {
        unsigned int content_size = sizeof(unsigned int) * 1026;
        void *content = malloc(content_size);
        int *temp = (int*)malloc(sizeof(int));

        *temp = this->max_block_per_group;
        memcpy(content, temp, sizeof(unsigned int));

        if(i == this->max_group_num -1)
            *temp = -1;
        else
            *temp = (i+1) * this->max_block_per_group;
        memcpy((char*)content + sizeof(unsigned int), temp, sizeof(unsigned int));

        for(unsigned int j=0; j<this->max_block_per_group; j++)
        {
            *temp = i * this->max_block_per_group + j;
            memcpy((char*)content + sizeof(unsigned int)*(2+j), temp, sizeof(unsigned int));
        }

        this->writeOneBlock(i * this->max_block_per_group, content, 0, content_size);
        free(content);
        free(temp);
    }


    qDebug() << "initialization done";
}

int SimulationDisk::allocateOneBlock()
{

    int block_idx;
    if(this->super_block->free_block_num != 0)
    {
        block_idx = this->super_block->blocks_idx_stack.top();
        this->super_block->blocks_idx_stack.pop();
        this->super_block->free_block_num --;
    }
    else
    {
        if(this->super_block->next_group_head_idx != -1)
        {
            this->first_block_idx = this->super_block->next_group_head_idx;
            this->ReadSuperBlock();
            block_idx = this->super_block->blocks_idx_stack.top();
            this->super_block->blocks_idx_stack.pop();
            this->super_block->free_block_num --;
        }
        else
        {
            block_idx = -1;
        }
    }

    if(block_idx != -1)
        this->remain_block_num --;
    return block_idx;
}

void SimulationDisk::freeOneBlock(unsigned int index)
{
    if(this->super_block->free_block_num != this->max_block_per_group)
    {
        this->super_block->blocks_idx_stack.push(index);
        this->super_block->free_block_num ++;
    }
    else
    {
        this->WriteSuperBlock();
        this->super_block->blocks_idx_stack.push(index);
        this->super_block->free_block_num = 1;
    }

    this->remain_block_num ++;
}

void SimulationDisk::LoadDisk()
{
    if(this->medium->is_empty == true) //Disk为空，初始化Disk
    {
        this->medium->first_block_idx = 0;
        this->first_block_idx = 0;
        this->Initialization();
        this->medium->is_empty = false;
    }
    else //从PhysicalMedium加载Disk
    {
        this->medium->LoadDiskContent(this->disk_head, this->total_byte_num);
    }
}

void SimulationDisk::SaveDisk()
{
    this->WriteSuperBlock();

    this->medium->remain_block_num = this->remain_block_num;
    this->medium->first_block_idx = this->first_block_idx;

    this->medium->SaveDiskInfo();
    this->medium->SaveDiskContent(this->disk_head, this->total_byte_num);
}


void SimulationDisk::ReadSuperBlock()
{
    unsigned int read_size = 1026 * sizeof(unsigned int);
    void* group_info = malloc(read_size);
    this->readOneBlock(this->first_block_idx, group_info, read_size);

    int *temp = (int*)malloc(sizeof(int));

    memcpy(temp, group_info, sizeof(unsigned int));
    this->super_block->free_block_num = *temp;

    memcpy(temp, (char*)group_info + sizeof(unsigned int), sizeof(unsigned int));
    this->super_block->next_group_head_idx = *temp;

    for(unsigned int i=0; i<this->super_block->free_block_num; i++)
    {
        memcpy(temp, (char*)group_info + sizeof(unsigned int) * (2+i), sizeof(unsigned int));
        this->super_block->blocks_idx_stack.push(*temp);
    }

    free(group_info);
    free(temp);
}

void SimulationDisk::WriteSuperBlock()
{
    unsigned int write_size = 1026 * sizeof(unsigned int);
    void *group_info = malloc(write_size);
    int *temp = (int*)malloc(sizeof(int));

    *temp = this->super_block->free_block_num;
    memcpy(group_info, temp, sizeof(unsigned int));

    *temp = this->super_block->next_group_head_idx;
    memcpy((char*)group_info + sizeof(unsigned int), temp, sizeof(unsigned int));

    unsigned int last_block_idx = this->super_block->blocks_idx_stack.top();

    for(unsigned int i=0; i<this->super_block->free_block_num; i++)
    {
        *temp = this->super_block->blocks_idx_stack.top();
        this->super_block->blocks_idx_stack.pop();
        memcpy((char*)group_info + sizeof(unsigned int) * (2+i), temp, sizeof(unsigned int));
    }

    this->first_block_idx = last_block_idx;
    this->writeOneBlock(this->first_block_idx, group_info, 0, write_size);
}
