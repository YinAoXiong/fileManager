//磁盘块数据结构

#include<map>
#include<vector>
#include<QTreeWidget>
#include<QString>
#include<QMessageBox>
#include "emptyblocklist.h"

using namespace std;
#ifndef DISKBLOCK_H
#define DISKBLOCK_H

class DiskBlock
{
private:
    static const int RECORDSIZE=64;
    short int flag=-1;   //文件为0，目录为1
    int emptySize=1024; //磁盘的空闲空间大小，单位b，大小为1kb
    QString fileContent;
    map<QString,int> fcb;
    int nextBlock=-1;

public:
    static const short int FILE=0;
    static const short int DIR=1;
    DiskBlock();
    void clear();
    void setFlag(short int flag);
    int newDIR(QString name, EmptyBlockList & emptyBlockList, vector<DiskBlock> &disk, QTreeWidgetItem *item);
    int newFile(QString name, EmptyBlockList & emptyBlockList, vector<DiskBlock> &disk,QTreeWidgetItem *item);
    //递归查看文件是否存在
    int fileExist(QString name,vector<DiskBlock> &disk);
    int getNextBlock();
    QString getFileContent();
    void setFileContent(QString content);
    short int getFlag();
    map<QString, int> getFCB();
    void deleteRecord(QString name);
};

#endif // DISKBLOCK_H
