#include "file.h"
#include <QTime>
#include <QDate>
#include <string>
#include <cmath>

File::File(unsigned int index_block, SimulationDisk *disk)
{
    this->disk = disk;
    this->index_block = index_block;
    this->blocks_list = new std::vector<unsigned>;
    this->access_count = 0;
    this->ReadIndexBlock();
}

File::File(std::string name, std::string owner, bool is_dir, SimulationDisk *disk)
{
    this->name = name;
    this->size = 0;
    this->owner = owner;
    this->is_dir = is_dir;
    this->create_time = QDate::currentDate().toString("yyyy-MM-dd").toStdString() + " " + QTime::currentTime().toString("hh:mm:ss").toStdString();
    this->modify_time = this->create_time;

    this->disk = disk;
    this->index_block = this->disk->allocateOneBlock();
    this->blocks_num = 0;
    this->blocks_list = new std::vector<unsigned>;
    this->access_count = 0;

    this->WriteIndexBlock();
}

void File::ReadIndexBlock()
{
    void *content = malloc(this->disk->block_size);
    char *temp1 = (char*)malloc(128);
    int  *temp2 = (int*)malloc(4);
    this->disk->readOneBlock(this->index_block, content, this->disk->block_size);

    //name
    memcpy(temp1, content, 128);
    this->name = temp1;
    //size
    memcpy(temp2, (char*)content + 128, 4);
    this->size = *temp2;
    //owner
    memcpy(temp1, (char*)content + 132, 64);
    this->owner = temp1;
    //is_dir
    memcpy(temp2, (char*)content + 196, 4);
    this->is_dir = *temp2;
    //create_time
    memcpy(temp1, (char*)content + 200, 32);
    this->create_time = temp1;
    //modify_time
    memcpy(temp1, (char*)content + 232, 32);
    this->modify_time = temp1;
    // blocks_num
    memcpy(temp2, (char*)content + 264, 4);
    this->blocks_num = *temp2;
    //blocks_list
    for(unsigned i=0; i<this->blocks_num; i++)
    {
        memcpy(temp2, (char*)content + 268 + i*4, 4);
        this->blocks_list->push_back(*temp2);
    }

    free(content);
    free(temp1);
    free(temp2);
}

void File::WriteIndexBlock()
{
    void *content = malloc(this->disk->block_size);
    char *temp1 = (char*)malloc(128);
    int  *temp2 = (int*)malloc(4);

    //name
    strcpy(temp1, this->name.c_str());
    memcpy(content, temp1, 128);
    //size
    *temp2 = this->size;
    memcpy((char*)content + 128, temp2, 4);
    //owner
    strcpy(temp1, this->owner.c_str());
    memcpy((char*)content + 132, temp1, 64);
    //is_dir
    *temp2 = this->is_dir;
    memcpy((char*)content + 196, temp2, 4);
    //create_time
    strcpy(temp1, this->create_time.c_str());
    memcpy((char*)content + 200, temp1, 32);
    //modify_time
    strcpy(temp1, this->modify_time.c_str());
    memcpy((char*)content + 232, temp1, 32);
    //blocks_num
    *temp2 = this->blocks_num;
    memcpy((char*)content + 264, temp2, 4);
    //blocks_list
    for(unsigned i=0; i<this->blocks_num; i++)
    {
        *temp2 = this->blocks_list->at(i);
        memcpy((char*)content + 268 + i*4, temp2, 4);
    }

    this->disk->writeOneBlock(this->index_block, content, 0, this->disk->block_size);
    free(content);
    free(temp1);
    free(temp2);
}


void File::FreeFileBlocks()
{
    if(this->is_dir == false) //非目录文件，直接删除
    {
        //回收文件体
        for(unsigned i=0; i<this->blocks_num; i++)
        {
            this->disk->freeOneBlock(this->blocks_list->at(i));
        }
        //回收文件头
        this->disk->freeOneBlock(this->index_block);
    }
    else //目录文件，递归删除
    {
        //回收目录中的文件
        for(unsigned i=0; i<this->blocks_num; i++)
        {
            File file(this->blocks_list->at(i), this->disk);
            file.FreeFileBlocks();
        }

        //回收文件头
        this->disk->freeOneBlock(this->index_block);
    }
}


bool File::WriteContent(void *content, int bytes)
{
    this->size = bytes;
    this->blocks_num = std::ceil( (float)bytes / (float)this->disk->block_size );
    /*申请磁盘块*/
    for(unsigned i=0; i<this->blocks_num; i++)
    {
        int block_idx = this->disk->allocateOneBlock();
        if(block_idx == -1){
            return false;
        }
        this->blocks_list->push_back(block_idx);
    }

    /*往磁盘块写文件体*/
    for(unsigned i=0; i<this->blocks_num; i++)
    {
        if(i != this->blocks_num - 1)
        {
            this->disk->writeOneBlock(this->blocks_list->at(i), content, i * this->disk->block_size, this->disk->block_size);
        }
        else
        {
            this->disk->writeOneBlock(this->blocks_list->at(i), content, i * this->disk->block_size, this->size - i * this->disk->block_size);
        }
    }
    return true;
}


void File::ReadContent(void *content)
{
    for(unsigned i=0; i<this->blocks_num; i++)
    {
        if(i != this->blocks_num - 1)
        {
            this->disk->readOneBlock(this->blocks_list->at(i), (char*)content + i * this->disk->block_size, this->disk->block_size);
        }
        else
        {
            this->disk->readOneBlock(this->blocks_list->at(i), (char*)content + i * this->disk->block_size, this->size - i * this->disk->block_size);
        }
    }
}
