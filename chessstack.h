#ifndef CHESSSTACK_H
#define CHESSSTACK_H
#include "node.h"

class chessStack
{
    //自定义栈
public:
    chessStack();
    void push(const node item);
    void pop();
    node* display();//返回list数组，板子输出的时候另外在字符串中添加
    void clear();
    void setSize(int m,int n);
    ~chessStack();

private:
    node* list;
    int size;
    int capacity;
};

#endif // CHESSSTACK_H
