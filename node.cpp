#include "node.h"

node::node() {
    x=1;y=1;
}

node::node(int x,int y){
    this->x=x;
    this->y=y;
}

int node::getX(){
    return x;
}

int node::getY(){
    return y;
}
