/*物理层*/

#ifndef PHYSICALMEDIUM_H
#define PHYSICALMEDIUM_H


class PhysicalMedium
{
public:
    PhysicalMedium(const char *save_dir); //构造函数

    /*磁盘信息*/
    unsigned int     block_size;       // 每个block有多少byte
    unsigned int     total_block_num;  //磁盘总block数
    unsigned int     remain_block_num; //空闲的block数
    unsigned int     first_block_idx;  //第0组第0块号码
    const char       *save_dir;        //保存路径
    bool             is_empty;         //磁盘是否为空盘

    /*磁盘加载与保存*/
    void LoadDiskInfo();    //从配置文件中加载磁盘属性
    void SaveDiskInfo();    //将磁盘属性保存到配置文件
    void LoadDiskContent(void *disk_head, unsigned int byte_num);   //从物理文件中加载虚拟磁盘
    void SaveDiskContent(void *disk_head, unsigned int byte_num);   //将虚拟磁盘保存到物理文件
};

#endif // PHYSICALMEDIUM_H
