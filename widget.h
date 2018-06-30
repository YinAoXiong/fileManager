#ifndef WIDGET_H
#define WIDGET_H

#include<vector>
#include <QWidget>
#include<QDebug>
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

private:
    Ui::Widget *ui;
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
    void deleteFile(int id, QTreeWidgetItem *item);
    //递归删除文件夹
    void deleteDIR(int id, QTreeWidgetItem *treeItem);
};

#endif // WIDGET_H
