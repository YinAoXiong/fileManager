#include "emptyblocklist.h"

EmptyBlockList::EmptyBlockList()
{

}

int EmptyBlockList::getEmptyBlock()
{
    if(blockData.size()==0)
    {
        QMessageBox::warning(NULL,"警告","磁盘储存空间不足");
        return -1;
    }
    int result=blockData.front();
    blockData.pop_front();
    return result;
}

void EmptyBlockList::addEmptyBlock(int id)
{
    blockData.push_back(id);
}

int EmptyBlockList::size()
{
    return blockData.size();
}
