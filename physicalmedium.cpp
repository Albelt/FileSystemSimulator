#include "physicalmedium.h"
#include <cstring>
#include <fstream>
#include <QDebug>

PhysicalMedium::PhysicalMedium(const char *save_dir)
{
    this->save_dir = save_dir;
}

void PhysicalMedium::LoadDiskInfo()
{
    char info_file[100];
    strcpy(info_file, this->save_dir);
    strcat(info_file, "disk_info");

    std::fstream fs;
    char buffer[1024];
    std::string temp;

    fs.open(info_file, std::fstream::in);
    if(! fs.is_open()){
        qDebug() << "can't open disk info file";
        exit(100);
    }

    fs.getline(buffer, 1024);
    temp = buffer;
    this->block_size = std::stoi(temp);

    fs.getline(buffer, 1024);
    temp = buffer;
    this->total_block_num = std::stoi(temp);

    fs.getline(buffer, 1024);
    temp = buffer;
    this->remain_block_num = std::stoi(temp);

    fs.getline(buffer, 1024);
    temp = buffer;
    this->first_block_idx = std::stoi(temp);

    fs.getline(buffer, 1024);
    temp = buffer;
    this->is_empty = std::stoi(temp);

    fs.close();
}

void PhysicalMedium::SaveDiskInfo()
{
    char info_file[100];
    strcpy(info_file, this->save_dir);
    strcat(info_file, "disk_info");

    std::fstream fs;
    fs.open(info_file, std::fstream::out);
    if(! fs.is_open()){
        qDebug() << "can't open disk info file";
        exit(100);
    }
    fs << this->block_size << "\n" << this->total_block_num << "\n" << this->remain_block_num << "\n" << this->first_block_idx << "\n" << this->is_empty;
    fs.close();
}

void PhysicalMedium::LoadDiskContent(void *disk_head, unsigned int byte_num)
{
    char disk_file[100];
    strcpy(disk_file, this->save_dir);
    strcat(disk_file, "disk_content");

    std::fstream fs;
    fs.open(disk_file, std::fstream::in | std::fstream::binary);
    if(! fs.is_open()){
        qDebug() << "can't open disk content file";
        exit(100);
    }

    char *head = (char*)disk_head;
    fs.read(head, byte_num);
    fs.close();
}

void PhysicalMedium::SaveDiskContent(void *disk_head, unsigned int byte_num)
{
    char content_file[100];
    strcpy(content_file, this->save_dir);
    strcat(content_file, "disk_content");

    std::fstream fs;
    fs.open(content_file, std::fstream::out | std::fstream::binary);
    if(! fs.is_open()){
        qDebug() << "can't open disk content file";
        exit(100);
    }
    char *write_head = (char*)disk_head;
    fs.write(write_head, byte_num);
    fs.close();
}
