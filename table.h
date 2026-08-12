#ifndef TABLE_H
#define TABLE_H

class table{
public:
    int height,width;
    table(){
        this->height=10;
        this->width=9;
    }
    void setSize(int height,int width){
        this->height=height;
        this->width=width;
    }
    ~table(){

    }
};

#endif // TABLE_H
