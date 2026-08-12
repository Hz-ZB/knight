#include "chessstack.h"

chessStack::chessStack() {
    size=0;
    capacity=0;
}

void chessStack::setSize(int m,int n){
    list=new node[m*n+1];
    capacity=m*n;
}
void chessStack::push(const node item){
    if (capacity==size) return;
    list[++size]=item;
}
void chessStack::pop(){
    if (size==0) return;
    size--;
}
node* chessStack::display(){
    return list;
    /*
    for (int i=1;i<=size;i++){
        cout<<"("<<list[i].first<<","<<list[i].second<<")\t";
        if (i%9==0) cout<<endl;
    }
    cout<<endl;
   */
}
void chessStack::clear(){
    size=0;
}
chessStack::~chessStack(){
    delete[] list;
}
