#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //初始化磁盘数据
    disk.resize(trackNumber*blockNumber);
    //初始化空闲块链数据
    for(int i=0;i<trackNumber*blockNumber;++i)
    {
        emptyBlockList.addEmptyBlock(i);
    }

    int blockID=emptyBlockList.getEmptyBlock();
    disk[blockID].setFlag(DiskBlock::DIR);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_open_clicked()
{
    QString path=ui->path->text();
    if(path.size()==0)
        return;
    //对路径进行划分
    QStringList nameList=path.split("/");
    if(nameList.last()=="")
    {
        QMessageBox::critical(NULL,"警告","禁止直接编辑目录文件");
        return;
    }
    int i=0;
    if(nameList[0]=="")
        i=1;
    int loopTimes=nameList.size();
    int id=0;
    for(i;i<loopTimes;++i)
    {
        int diskID=disk[id].fileExist(nameList[i],disk);
        if(diskID==-1)
        {
            QMessageBox::critical(NULL,"警告","输入的路径有误");
            return;
        }
        id=diskID;
    }
    inodeID=id;
    ui->fileName->setText(nameList.last());
    QString content;
    while (id>=0) {
        content+=disk[id].getFileContent();
        id=disk[id].getNextBlock();
    }
    ui->fileContent->setText(content);
}

QTreeWidgetItem * Widget::findItem(QTreeWidgetItem *item, QString name)
{
    QTreeWidgetItem * result=NULL;
    int loopTimes=item->childCount();
    for(int i=0;i<loopTimes;++i)
    {
        result=item->child(i);
        if(result->text(0)==name)
            return result;
    }
    result=NULL;
    return result;
}

void Widget::on_newFile_clicked()
{
    QString path=ui->path->text();
    if(path.size()==0)
        return;
    //对路径进行分割
    QStringList nameList=path.split("/");
    //判断是创建文件夹还是创建文件,并且根据输入路径对文件进行递归创建
    if(nameList[nameList.size()-1].size()==0)
    {
        int i=0;
        if(nameList[0]=="")
            i=1;
        int loopTimes=nameList.size()-1;
        int id=0;
        QTreeWidgetItem * item=ui->fileTree->topLevelItem(0);
        for(i;i<loopTimes;++i)
        {
            int diskID=disk[id].fileExist(nameList[i],disk);
            if(diskID>=0)
            {
                item=findItem(item,nameList[i]);
                id=diskID;
            }
            else
            {
                diskID=disk[id].newDIR(nameList[i],emptyBlockList,disk,item);
                if(diskID>=0)
                {
                    item=findItem(item,nameList[i]);
                    id=diskID;
                }

            }
        }
    }else
    {
        int i=0;
        if(nameList[0]=="")
            i=1;
        int loopTimes=nameList.size()-1;
        int id=0;
        QTreeWidgetItem * item=ui->fileTree->topLevelItem(0);
        for(i;i<loopTimes;++i)
        {
            int diskID=disk[id].fileExist(nameList[i],disk);
            if(diskID>=0)
            {
                item=findItem(item,nameList[i]);
                id=diskID;
            }
            else
            {
                diskID=disk[id].newDIR(nameList[i],emptyBlockList,disk,item);
                if(diskID>=0)
                {
                    item=findItem(item,nameList[i]);
                    id=diskID;
                }

            }
        }

        disk[id].newFile(nameList[nameList.size()-1],emptyBlockList,disk,item);
    }
}

void Widget::on_save_clicked()
{
    int id=inodeID;
    QString content=ui->fileContent->toPlainText();
    while (true) {
        //一个字符一个字节，一个block可以存放128个字符
        if(content.size()<=128)
        {
            disk[id].setFileContent(content);
            break;
        }else {
            disk[id].setFileContent(content.mid(128));
            content=content.mid(128,content.size());
        }
        if(disk[id].getNextBlock()>=0)
        {
            id=disk[id].getNextBlock();
        }else
        {
            id=emptyBlockList.getEmptyBlock();
            if(id<0)
                return;
            disk[id].setFlag(DiskBlock::FILE);
        }
    }
    //释放可能存在的剩余节点
    id=disk[id].getNextBlock();
    while (id>=0) {
        int nextBlock=disk[id].getNextBlock();
        disk[id].clear();
        emptyBlockList.addEmptyBlock(id);
        id=nextBlock;
    }
}

void Widget::deleteFile(int id,QTreeWidgetItem * item)
{
    if(id==inodeID)
    {
        QString fileName=item->text(0);
        QMessageBox::warning(NULL,"警告",tr("文件 %1 正在被编辑无法删除").arg(fileName));
        return;
    }
    //删除文件的树形控件
    delete item;
    //释放磁盘空间
    while (id>=0) {
        int nextBlock=disk[id].getNextBlock();
        disk[id].clear();
        emptyBlockList.addEmptyBlock(id);
        id=nextBlock;
    }
}

void Widget::deleteDIR(int id,QTreeWidgetItem * treeItem)
{
    while (id>=0) {
        int nextBlock=disk[id].getNextBlock();
        map<QString,int> fcb=disk[id].getFCB();
        for(auto item:fcb)
        {
            if(disk[item.second].getFlag()==DiskBlock::FILE)
            {
                QTreeWidgetItem * tempItem=findItem(treeItem,item.first);
                deleteFile(item.second,tempItem);
                disk[id].deleteRecord(item.first);
            }else
            {
                QTreeWidgetItem * tempItem=findItem(treeItem,item.first);
                deleteDIR(item.second,tempItem);
            }
        }
        disk[id].clear();
        emptyBlockList.addEmptyBlock(id);
        id=nextBlock;
    }
    delete treeItem;
}

void Widget::on_deleteFile_clicked()
{
    QString path=ui->path->text();
    if(path.size()==0)
        return;
    //对输入路径进行划分
    QStringList nameList=path.split("/");
    //判断是文件夹还是文件
    if(nameList.last()=="")
    {
        int i=0;
        if(nameList[0]=="")
            i=1;
        int loopTimes=nameList.size()-1;
        int id=0;
        QTreeWidgetItem * item=ui->fileTree->topLevelItem(0);
        for(i;i<loopTimes;++i)
        {
            int diskID=disk[id].fileExist(nameList[i],disk);
            if(diskID>=0)
            {
                item=findItem(item,nameList[i]);
                id=diskID;
            }else
            {
                QMessageBox::critical(NULL,"警告","你输入的路径有误");
                return;
            }
        }

    }else
    {

        int i=0;
        if(nameList[0]=="")
            i=1;
        int loopTimes=nameList.size()-1;
        int id=0;
        QTreeWidgetItem * item=ui->fileTree->topLevelItem(0);
        for(i;i<loopTimes;++i)
        {
            int diskID=disk[id].fileExist(nameList[i],disk);
            if(diskID>=0)
            {
                item=findItem(item,nameList[i]);
                id=diskID;
            }else
            {
                QMessageBox::critical(NULL,"警告","你输入的路径有误");
                return;
            }
        }

        item=findItem(item,nameList.last());
        int diskID=disk[id].fileExist(nameList.last(),disk);
        if(diskID>=0)
        {
            deleteFile(diskID,item);
            disk[id].deleteRecord(nameList.last());
        }else
        {
            QMessageBox::critical(NULL,"警告","你输入的路径有误");
        }
    }
}

void Widget::on_close_clicked()
{
    inodeID=-1;
    ui->fileName->setText("没有打开文件");
    ui->fileContent->setText("");
}
