#ifndef WIDGET_H
#define WIDGET_H

#include<vector>
#include<QWidget>
#include<QDebug>
#include <QTimer>
#include "diskblock.h"
#include "emptyblocklist.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_open_clicked();
    void on_newFile_clicked();

    void on_save_clicked();

    void on_deleteFile_clicked();

    void on_close_clicked();

    void updateGUI();

private:
    Ui::Widget *ui;
    //记录当前磁道移动方向，true为正向false为反向
    bool direction=true;
    //当前访问磁道
    int trackID=0;
    //最后的磁道号
    int trackEnd=0;
    //开始的磁道号
    int trackBegin=127;
    //更新访问磁道与磁盘用的定时器
    QTimer * updateTimer;
    //FCFS中使用的数据结构
    list<int> waitIDByList;
    vector<vector<int>> waitIDByVector;
    //SCAN和CSCAN中
    vector<DiskBlock> disk;
    EmptyBlockList emptyBlockList;
    //磁道数
    int trackNumber=128;
    //每个磁道上的盘块数
    int blockNumber=8;
    //文件inode节点id
    int inodeID=-1;
    QTreeWidgetItem * findItem(QTreeWidgetItem * item,QString name);
    //删除单个文件
    int deleteFile(int id, QTreeWidgetItem *item);
    //递归删除文件夹
    void deleteDIR(int id, QTreeWidgetItem *treeItem);
    //添加待访问的磁盘块
    void addWaitBlock(int id);
};

#endif // WIDGET_H
