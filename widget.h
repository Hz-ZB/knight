#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "knightsolver.h"
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QMap>
#include <QList>
#include <QPaintEvent>
#include <QFrame>
#include "table.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr,table* t=nullptr);
    ~Widget();
    void stop();
    void safelyExit(){
        on_exitButton_clicked();
    }


private slots:
    void on_nextButton_clicked();

    void on_exitButton_clicked();

    void on_autoButton_clicked();

    void on_clearButton_clicked();

    void on_testButton_clicked();

    void on_speedBox_currentIndexChanged(int index);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;


private:
    Ui::Widget *ui;
    QFrame *drawFrame;//指向frame控件
    int index=0;
    bool isStarted;
    bool finished;
    bool isAuto;//是否正处于自动遍历模式
    int n,m;//高度，宽度
    const int gridHeight=66,gridWidth=71;//格子的尺寸与初始位置
    const int beginHeight=30,beginWidth=50;
    knightSolver* knight;//求解器
    QString route;
    node* list;//存储遍历节点

    QPixmap imgKnight; //棋子图片
    QPixmap imgGreen; //绿色圆圈（已遍历）
    QPixmap imgRed; //红叉（可走节点）
    QPixmap imgGrey;//幕布（半个棋盘时遮住下半部分）
    QPixmap imgGrey2;//幕布（半个棋盘时遮住右半部分）

    QPoint curKnightPos;// 当前马的像素坐标，保证仅一个马
    QHash<QPoint, int> visitedPos; // 已遍历位置：键=像素坐标，值=遍历索引
    QList<QPoint> validPos;// 当前可走位置（红叉），每次移动后清空

    //私有方法
    void initImage();//图像初始化
    QPoint getGridCenter(int x, int y); // 依据坐标获得像素中心坐标
    void calcValidPos(); // 计算当前位置的可走红叉位置
    void adjustLabel();//调整坐标轴的尺寸

    int INTERVAL;

};
#endif // WIDGET_H
