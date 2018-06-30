//空闲块链

#include<list>
#include<QMessageBox>

using namespace std;

#ifndef EMPTYBLOCKLIST_H
#define EMPTYBLOCKLIST_H


class EmptyBlockList
{
private:
    list<int> blockData;
public:
    EmptyBlockList();
    int getEmptyBlock();
    void addEmptyBlock(int id);
    int size();
};

#endif // EMPTYBLOCKLIST_H
