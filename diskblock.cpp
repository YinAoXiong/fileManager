#include "diskblock.h"

DiskBlock::DiskBlock()
{

}

void DiskBlock::clear()
{
    flag=-1;
    fileContent.clear();
    fcb.clear();
    emptySize=1024;
    nextBlock=-1;
}

void DiskBlock::setFlag(short int flag)
{
    this->flag=flag;
}

int DiskBlock::newDIR(QString name, EmptyBlockList & emptyBlockList, vector<DiskBlock>& disk,QTreeWidgetItem * item)
{
    //先判断该磁盘块是否有足够的空间进行存储,不够则申请新的空间
    if(emptySize>0)
    {
        if(flag!=1)
            return -1;
        //查看新建的文件是否已经存在
        if(fcb.end()!=fcb.find(name))
        {
            QMessageBox::critical(NULL,"警告","同名文件已存在");
            return -1;
        }
        //获取空闲磁盘块并设置类型为目录类型
        int blockID=emptyBlockList.getEmptyBlock();
        if(blockID>=0)
        {
            disk[blockID].setFlag(DiskBlock::DIR);
            //将新目录写入map并更新磁盘块空闲大小
            fcb[name]=blockID;
            emptySize-=RECORDSIZE;
            //更新树状图
            QTreeWidgetItem * newItem=new QTreeWidgetItem(QStringList(name));
            newItem->setIcon(0,QIcon(QStringLiteral(":/dir.ico")));
            item->addChild(newItem);
            return blockID;
        }
    }
    else
    {
        int blockID=emptyBlockList.getEmptyBlock();
        if(blockID>=0)
        {
            disk[blockID].setFlag(DiskBlock::DIR);
            nextBlock=blockID;
            return disk[blockID].newDIR(name,emptyBlockList,disk,item);
        }
    }
    //表示没有创建成功
    return -1;
}

int DiskBlock::newFile(QString name, EmptyBlockList &emptyBlockList, vector<DiskBlock> &disk, QTreeWidgetItem *item)
{
    if(emptySize>0)
    {
        if(flag!=1)
            return -1;
        //查看新建的文件是否已经存在
        if(fcb.end()!=fcb.find(name))
        {
            QMessageBox::critical(NULL,"警告","同名文件已存在");
            return -1;
        }
        //获取空闲磁盘块并设置类型为文件类型
        int blockID=emptyBlockList.getEmptyBlock();
        if(blockID>=0)
        {
            disk[blockID].setFlag(DiskBlock::FILE);
            //将新文件写入map并更新磁盘块空闲大小
            fcb[name]=blockID;
            emptySize-=RECORDSIZE;
            //更新树状图
            QTreeWidgetItem * newItem=new QTreeWidgetItem(QStringList(name));
            newItem->setIcon(0,QIcon(QStringLiteral(":/file.ico")));
            item->addChild(newItem);
            return blockID;
        }

    }else
    {
        int blockID=emptyBlockList.getEmptyBlock();
        if(blockID>=0)
        {
            disk[blockID].setFlag(DiskBlock::FILE);
            nextBlock=blockID;
            return disk[blockID].newFile(name,emptyBlockList,disk,item);
        }
    }
    return -1;
}

int DiskBlock::fileExist(QString name, vector<DiskBlock> &disk)
{
    if(fcb.end()!=fcb.find(name))
    {
        return fcb[name];
    }
    if(nextBlock==-1)
        return -1;
    return fileExist(name,disk);
}

int DiskBlock::getNextBlock()
{
    return nextBlock;
}

QString DiskBlock::getFileContent()
{
    return fileContent;
}

void DiskBlock::setFileContent(QString content)
{
    fileContent=content;
    emptySize=1024-8*content.size();
}

short int DiskBlock::getFlag()
{
    return flag;
}

map<QString,int> DiskBlock::getFCB()
{
    return fcb;
}

void DiskBlock::deleteRecord(QString name)
{
    fcb.erase(name);
}
