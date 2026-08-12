#include "knightsolver.h"

knightSolver::knightSolver(const class table* t) {
    int n,m;
    m=t->height;
    n=t->width;
    this->width=n;
    this->height=m;
    route.setSize(width,height);
    table=new int*[height+1];
    canReach=new int*[height+1];
    for (int i=1;i<=height;i++){
        table[i]=new int[width+1];
        canReach[i]=new int[width+1];
    }
    clear();
}

bool knightSolver::OutOfRange(int x,int y){
    if (x<1 || x>height || y<1 || y>width || table[x][y]!=UNREACHED){
        return true;
    }
    return false;
}

void knightSolver::clear(){
    for (int i=1;i<=height;i++){
        for (int j=1;j<=width;j++){//初始时每个节点均未被访问
            table[i][j]=UNREACHED;
            canReach[i][j]=0;
        }
    }
    canReachInitialize();
    route.clear();
    finished=false;
    traceCount=0;
    count=0;//计算时间复杂度
}

void knightSolver::solve(int x,int y,int cnt){
    if (traceCount>=MAX_TRACE) return;
    table[x][y]=cnt;
    count++;
    traceCount++;
    route.push(node(x,y));//首先完成该节点的标记
    if (cnt==height*width){
        finished=true;//结束判断
        return;
    }
    bool Labeled[9]={0};
    for (int i=1;i<=DIRECTION;i++){
        int nx=x+dx[i],ny=y+dy[i];
        if (OutOfRange(nx,ny)) continue;
        canReach[nx][ny]--;//相应节点能去的节点数-1
    }
    //搜索下一个能去的节点
    for (int i=1;i<=DIRECTION;i++){
        int target=-1;
        int tx,ty;
        for (int j=1;j<=DIRECTION;j++){
            if (Labeled[j]) continue;//已访问
            int nx=x+dx[j],ny=y+dy[j];
            if (OutOfRange(nx,ny)) continue;
            if (target==-1) {
                target=j;tx=nx;ty=ny;continue;
            }
            if (canReach[nx][ny]<=canReach[tx][ty]){
                //找到目前能去的节点数最少的节点，减少后续退回，其中误差可以接受
                tx=nx;ty=ny;target=j;
            }
        }
        if (target==-1)  break;//所有的节点全都去不了
        Labeled[target]=1;
        solve(tx,ty,cnt+1);//递归查找
        if (finished)
            return;//查找完成，不必再进入下一次循环
    }
    table[x][y]=UNREACHED;//未查找成功，恢复原始状态，递归退回
    route.pop();
    for (int i=1;i<=DIRECTION;i++){
        int nx=x+dx[i],ny=y+dy[i];
        if (OutOfRange(nx,ny)) continue;
        canReach[nx][ny]++;
    }
}

void knightSolver::trace(int x,int y){
    if (OutOfRange(x,y)){
        return;
    }
    solve(x,y,1);
}

node* knightSolver::showRoute(){
    if (finished)
        return route.display();
    return nullptr;
}

void knightSolver::canReachInitialize(){
    for (int i=1;i<=height;i++){
        for (int j=1;j<=width;j++){
            for (int k=1;k<=DIRECTION;k++){
                int px=i+dx[k],py=j+dy[k];
                if (!OutOfRange(px,py)){
                    canReach[i][j]++;//能够到达的节点数+1
                }
            }
        }
    }
}

int knightSolver::test(int x,int y){
    //测试节点出发的效率
    clear();
    trace(x,y);
    if (!finished) return 0;
    return traceCount;
}

bool knightSolver::hasFound(){
    return finished;
}

knightSolver::~knightSolver() {
    for (int i=1; i<=height;i++) {
        delete[] table[i];
        delete[] canReach[i];
    }
    delete[] table;
    delete[] canReach;
}
