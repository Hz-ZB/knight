#ifndef KNIGHTSOLVER_H
#define KNIGHTSOLVER_H
#include "chessstack.h"
#include "table.h"

class knightSolver
{
public:
    knightSolver(const table* t);
    void trace(int x,int y);//查找路径
    void clear();//清空当前搜索记录
    node* showRoute();//展示路径
    int test(int x,int y);
    bool hasFound();//是否有解
    ~knightSolver();

private:
    int width,height;//横向、纵向长度
    int** table;//存储每个节点被遍历到的轮次
    int** canReach;//能够到达这个节点的节点数目，便于贪心查找
    bool finished;//是否已完成查找
    chessStack route;//存储最终的路径
    int traceCount;//统计搜索的累计次数，后续拓展查找效率时可用
    int count=0;//当前遍历的次数

    static const int UNREACHED=0;//未走过的标识
    static const int DIRECTION=8;//每个棋子能向八个方向行走
    static const int MAX_TRACE=1e6;//最多访问次数，超过后默认访问失败
    const int dx[9]={0,1,1,2,2,-1,-1,-2,-2};
    const int dy[9]={0,2,-2,1,-1,2,-2,1,-1};

    bool OutOfRange(int x,int y);//节点是否超出棋盘或已遍历无法再次访问
    void solve(int x,int y,int cnt);//查找路径的递归方法
    void canReachInitialize();//初始化canReach表格
};

#endif // KNIGHTSOLVER_H
