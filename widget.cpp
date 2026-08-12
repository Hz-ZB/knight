#include "widget.h"
#include "ui_widget.h"
#include <cstdlib>
#include <cstring>
#include <QMouseEvent>
#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QDebug>

#define KNIGHT_PATH ":/Image/knight.png"
#define REACHED ":/Image/reached.png"
#define REACHABLE ":/Image/reachable.png"
#define BACKGROUND ":/Image/background.jpg"
#define UNUSED ":/Image/unused.png"
#define SLOWINTERVAL 700
#define MIDDLEINTERVAL 500
#define FASTINTERVAL 300
#define MaxHeight 10
#define MaxWidth 9

Widget::Widget(QWidget *parent,table* t)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    drawFrame = ui->frame; // 关联UI里的frame控件
    drawFrame->installEventFilter(this);// 给frame安装事件过滤器，拦截它的paintEvent

    //导入棋盘大小
    table* m_table=t;
    if (m_table==nullptr){
        m_table=new class table();
    }
    knight=new knightSolver(m_table);
    n=m_table->height;
    m=m_table->width;
    adjustLabel();

    INTERVAL=MIDDLEINTERVAL;
    isStarted=false;
    finished=false;
    isAuto=false;
    list=new node[n*m+10];
    route="途经路径【坐标:（行索引，列索引）】：\n";
    QString pred="请选择起点";
    ui->step->setText(pred);
    ui->step->setReadOnly(true);
    ui->step->setAlignment(Qt::AlignCenter);
    ui->speed->setReadOnly(true);

    ui->nextButton->setEnabled(false);
    ui->autoButton->setEnabled(false);
    ui->position->setText(route);
    ui->position->setReadOnly(true);

    initImage();

    setWindowIcon(QIcon(":/picture.ico"));
}

Widget::~Widget()
{
    stop();
    delete ui;
    delete knight;
    delete[] list;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (isStarted) return;
    int y = (event->position().x()-beginWidth )/ gridWidth + 1;
    int x = (event->position().y()-beginHeight)/ gridHeight + 1;

    if (x >= 1 && x <= n && y >= 1 && y <= m) {
        knight->trace(x, y);

        //未找到路径处理
        if (!knight->hasFound()) {
            curKnightPos = getGridCenter(x,y);
            visitedPos.clear();
            validPos.clear();
            drawFrame->update();
            route=route+"\n规定时间内找不到路径，请【重置】并更换起点重试";
            ui->position->setText(route);
            finished=true;
            isStarted=true;
            return;
        }

        //将已有的路径复制
        node* list2=knight->showRoute();
        for (int i=1;i<=n*m;i++){
            list[i]=list2[i];
        }
        isStarted = true;
        index++;

        ui->nextButton->setEnabled(true);
        ui->nextButton->setDefault(true);
        ui->nextButton->setFocus(); //主动将焦点转移到按钮
        //ui->nextButton->setStyleSheet("border-image:url(:/Image/button.png)");
        ui->autoButton->setEnabled(true);

        QFont font;
        font.setFamily("华文行楷");
        font.setPointSize(14);
        font.setBold(false);
        ui->step->setFont(font);
        QString pred=QString("%1 / %2").arg(index).arg(n*m);
        ui->step->setText(pred);

        route=route+QString("%1.(%2,%3)    ").arg(index).arg(list[index].getX()-1).arg(list[index].getY()-1);
        ui->position->setText(route);

        //绘制逻辑
        curKnightPos = getGridCenter(list[index].getX(), list[index].getY());
        visitedPos.insert(curKnightPos, index);
        drawFrame->update();
        QApplication::processEvents(QEventLoop::AllEvents, 100);
        QThread::msleep(INTERVAL);
        QApplication::processEvents(QEventLoop::AllEvents, 100);

        calcValidPos();
        drawFrame->update();
    }

}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != drawFrame || event->type() != QEvent::Paint){
        return QWidget::eventFilter(watched, event);
    }
    QPainter painter(drawFrame); // 绘制目标改为frame
    painter.setRenderHint(QPainter::Antialiasing);
    QPixmap pixmap(BACKGROUND);
    painter.drawPixmap(this->rect(), pixmap);
    bool hasLabeled=false;

    //绘制棋盘的可用部分（当选用部分棋盘时）
    if (n<MaxHeight){
        QPoint curtain=getGridCenter(n,1)-QPoint(gridWidth/2+5,-7);
        //painter.setOpacity(0.95);
        painter.drawPixmap(curtain, imgGrey);

        QFont font;
        font.setFamily("华文新魏");
        font.setPointSize(25);
        font.setBold(false);
        painter.setFont(font);
        painter.setPen(Qt::black);
        QString tmp="该区域不可用";
        QRect shadowRect(
            curtain.x(),
            curtain.y(),
            imgGrey.width(),
            imgGrey.height()
            );
        painter.drawText(shadowRect, Qt::AlignCenter, tmp);
        hasLabeled=true;
    }
    if (m<MaxWidth){
        QPoint curtain=getGridCenter(1,m)-QPoint(-1,gridHeight/2+10);
        //painter.setOpacity(0.95);
        painter.drawPixmap(curtain, imgGrey2);
        if (!hasLabeled){
            QFont font;
            font.setFamily("华文新魏");
            font.setPointSize(25);
            font.setBold(false);
            painter.setFont(font);
            painter.setPen(Qt::black);
            QString tmp="该\n区\n域\n不\n可\n用";
            QRect shadowRect(
                curtain.x(),
                curtain.y(),
                imgGrey2.width(),
                imgGrey2.height()
                );
            painter.drawText(shadowRect, Qt::AlignCenter, tmp);
        }
    }

    if(!isStarted)
        return QWidget::eventFilter(watched, event);

    // 绘制已遍历位置
    QFont font;
    font.setFamily("华文行楷");
    font.setPointSize(20);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(Qt::red);
    painter.setOpacity(0.85);
    for(auto it = visitedPos.begin(); it != visitedPos.end(); it++)
    {
        QPoint pos = it.key();
        int idx = it.value();
        QRect greenRect(
            pos.x() - imgGreen.width()/2,
            pos.y() - imgGreen.height()/2,
            imgGreen.width(),
            imgGreen.height()
            );
        painter.drawPixmap(greenRect, imgGreen);
        painter.drawText(greenRect, Qt::AlignCenter, QString::number(idx));
    }

    // 绘制当前位置的马
    painter.setOpacity(1);
    if(!curKnightPos.isNull())
    {
        QPoint knightTop = curKnightPos - QPoint(imgKnight.width()/2, imgKnight.height()/2);
        painter.drawPixmap(knightTop, imgKnight);
    }

    // 绘制当前可走节点
    painter.setOpacity(0.45);
    for(auto it = validPos.cbegin(); it != validPos.cend(); ++it)
    {
        const QPoint& pos = *it;
        QPoint redTop = pos - QPoint(imgRed.width()/2, imgRed.height()/2);
        painter.drawPixmap(redTop, imgRed);
    }

    return true;
}

void Widget::on_nextButton_clicked()
{
    if (finished){
        curKnightPos = QPoint();
        visitedPos.clear();
        validPos.clear();
        drawFrame->update();
        QMessageBox::warning(this, "提示", "遍历已完成，即将为你重置界面");
        on_clearButton_clicked();
        return;
    }

    //可走方向大于一个时，先选中下一个节点
    if (validPos.size()>1){
        validPos.clear();
        int x = list[index+1].getX();
        int y = list[index+1].getY();
        validPos.append(getGridCenter(x,y));
        drawFrame->update();
        QApplication::processEvents(QEventLoop::AllEvents, 100);
        QThread::msleep(INTERVAL);
        QApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    //移动事件处理
    if (finished) return;
    visitedPos.insert(curKnightPos, index);
    index++;
    QString pred=QString("%1 / %2").arg(index).arg(n*m);
    ui->step->setText(pred);
    if (index==n*m){
        ui->nextButton->setEnabled(false);
        ui->autoButton->setEnabled(false);
        finished=true;
        ui->step->deselect();
        ui->speed->deselect();
        // 将焦点转移到非文本控件（比如退出按钮），避免step获取焦点
        ui->exitButton->setFocus();
        // 禁用step的焦点获取能力（可选，彻底防止选中）
        ui->step->setFocusPolicy(Qt::NoFocus);
        ui->speed->setFocusPolicy(Qt::NoFocus);
    }
    curKnightPos = getGridCenter(list[index].getX(), list[index].getY());

    //输出板格式调整
    route=route+QString("%1.(%2,%3)   ").arg(index).arg(list[index].getX()-1).arg(list[index].getY()-1);
    if (index<10){
        route=route+" ";
    }
    if (index%5==0){
        route=route+"\n";
    }
    if (finished){
        route=route+"\n遍历结束，点击【重置】清空棋盘，点击【退出】关闭页面";
    }
    ui->position->setText(route);

    //棋子移动绘制
    validPos.clear();
    drawFrame->update();

    if (finished) return;//结束后无需加载可去节点

    QApplication::processEvents(QEventLoop::AllEvents, 100);
    QThread::msleep(INTERVAL);
    QApplication::processEvents(QEventLoop::AllEvents, 100);

    //计算可去节点
    calcValidPos();
    drawFrame->update();
}

void Widget::on_exitButton_clicked()
{
    stop();
    this->close();
}

void Widget::on_autoButton_clicked()
{
    isAuto = !isAuto;
    if (isAuto) {
        ui->speedBox->setEnabled(false);
        ui->autoButton->setText("暂停");
        ui->nextButton->setEnabled(false);
        ui->testButton->setEnabled(false);
        ui->clearButton->setEnabled(false);
        ui->exitButton->setEnabled(false);
        while (!finished && isStarted && isAuto) {
            on_nextButton_clicked();
            if (finished || index >= n*m) break;
            QApplication::processEvents(QEventLoop::AllEvents, 100);
            QThread::msleep(INTERVAL);
            QApplication::processEvents(QEventLoop::AllEvents, 100);
        }
        if (!finished) {
            ui->autoButton->setText("自动运行");
        }
    } else {
        ui->speedBox->setEnabled(true);
        ui->autoButton->setText("自动运行");
        ui->nextButton->setEnabled(true);
        ui->testButton->setEnabled(true);
        ui->clearButton->setEnabled(true);
        ui->exitButton->setEnabled(true);
    }

    if (finished){
        ui->speedBox->setEnabled(true);
        ui->autoButton->setText("自动运行");
        ui->testButton->setEnabled(true);
        ui->clearButton->setEnabled(true);
        ui->exitButton->setEnabled(true);
        return;
    }
    ui->nextButton->setDefault(true);
    ui->nextButton->setFocus(); // 主动将焦点转移到按钮
}

void Widget::on_clearButton_clicked()
{
    isStarted=false;
    finished=false;
    isAuto=false;
    index=0;
    knight->clear();

    if (list != nullptr) {
        delete[] list;
        list = new node[n*m+10];
    }
    route="途经路径【坐标:（行索引，列索引）】：\n";

    QFont font;
    font.setFamily("华文新魏");
    font.setPointSize(14);
    font.setBold(false);
    ui->step->setFont(font);
    QString pred="请选择起点";
    ui->step->setText(pred);

    //ui->nextButton->setStyleSheet("border-image:url(:/Image/button.png)");
    ui->nextButton->setEnabled(false);
    ui->nextButton->setDefault(false);
    ui->nextButton->clearFocus();
    ui->autoButton->setEnabled(false);
    ui->position->setText(route);
    ui->testButton->setEnabled(true);
    ui->step->setFocusPolicy(Qt::StrongFocus);
    ui->step->deselect();

    //把所有已绘制棋子删除
    curKnightPos = QPoint();
    visitedPos.clear();
    validPos.clear();
    drawFrame->update();

    QApplication::processEvents(QEventLoop::AllEvents, 100);
}

void Widget::on_testButton_clicked()
{
    on_clearButton_clicked();//先清空
    QString pred="查找中，请稍等";
    ui->step->setText(pred);
    int ans[12][11];
    int cnt=0;
    double sum=0;
    double fastCnt=0;
    for (int i=1;i<=n;i++){
        for (int j=1;j<=m;j++){
            if ((n*m)%2==1 && i%2!=j%2)
                ans[i][j]=0;
            else
                ans[i][j]=knight->test(i,j);
            if (ans[i][j]){
                sum+=ans[i][j];
                cnt++;
            }
            if (ans[i][j]==n*m){
                fastCnt++;
            }
        }
    }
    for (int i=1;i<=n;i++){
        for (int j=1;j<=m;j++){
            curKnightPos = getGridCenter(i, j);
            visitedPos.insert(curKnightPos, ans[i][j]);
        }
    }
    curKnightPos=QPoint();
    isStarted=true;
    finished=true;
    pred="查找完毕";
    ui->step->setText(pred);
    double k,rate;
    if (cnt==0){
        k=0,rate=0;
    }else{
        k=sum/cnt;
        rate=(n*m*100)/k;
        fastCnt=100*fastCnt/cnt;
    }
    QString tmp=QString("框内数字表示从该点出发搜索过程经过格点数目"
                          "\n\n显示【0】表示无法在规定时间内找到可行路径"
                          "\n\n共%0个节点有解，平均遍历次数%1"
                          "\n\n平均搜索效率%2\%，一次遍历成功率%3\%"
                          "\n\n点击【重置】清空棋盘，点击【退出】关闭页面")
                      .arg(cnt)
                      .arg(k, 0, 'f', 2)
                      .arg(rate, 0, 'f', 2)
                      .arg(fastCnt, 0, 'f', 2);
    ui->position->setText(tmp);
    drawFrame->update();
}

void Widget::initImage(){
    if(!imgKnight.load(KNIGHT_PATH)) qDebug() << "棋子图片加载失败";
    if(!imgGreen.load(REACHED)) qDebug() << "绿圈标识加载失败";
    if(!imgRed.load(REACHABLE)) qDebug() << "红叉标识加载失败";
    if (!imgGrey.load(UNUSED)) qDebug() << "下方不可用区域加载失败";
    if (!imgGrey2.load(UNUSED)) qDebug() << "右侧不可用区域加载失败";

    //若自定义棋盘，可修改灰色部分的大小
    int greyHeight=MaxHeight-n;
    //imgGrey = QPixmap(gridWidth*MaxWidth, gridHeight*greyHeight+17);
    imgGrey=imgGrey.scaled(gridWidth*MaxWidth-15, gridHeight*(greyHeight+0.5)+9, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    int greyWidth=MaxWidth-m;
    //imgGrey2 = QPixmap(gridWidth*greyWidth, gridHeight*n);
    imgGrey2=imgGrey2.scaled(gridWidth*(greyWidth+0.5)-21, gridHeight*n-15, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    if (greyHeight==0){
        imgGrey2=imgGrey2.scaled(gridWidth*(greyWidth+0.5)-21, gridHeight*n+27, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    //imgGrey.fill(QColor(220, 215, 205, 235));
    //imgGrey2.fill(QColor(220, 215, 205, 235));
    qreal scale = 0.9;
    qreal sk = 1.05;
    imgKnight = imgKnight.scaled(gridWidth*sk, gridHeight*sk, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imgGreen = imgGreen.scaled(gridWidth, gridHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imgRed = imgRed.scaled(gridWidth*scale, gridHeight*scale, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //imgGrey = imgGrey.scaled(gridWidth*8, gridHeight*4+20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QPoint Widget::getGridCenter(int x, int y)//根据坐标返回像素中心位置
{
    int px = beginWidth + (y-1)*gridWidth + gridWidth/2;
    int py = beginHeight + (x-1)*gridHeight + gridHeight/2;
    return QPoint(px, py);
}

void Widget::calcValidPos()
{
    validPos.clear(); // 清空上一步红叉
    if(curKnightPos.isNull() || finished) return;

    int x = list[index].getX();
    int y = list[index].getY();

    const int dx[9]={0,1,1,2,2,-1,-1,-2,-2};
    const int dy[9]={0,2,-2,1,-1,2,-2,1,-1};
    for(int i=1; i<=8; i++)
    {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if(nx>=1 && nx<=n && ny>=1 && ny<=m && !visitedPos.contains(getGridCenter(nx, ny)))
        {
            validPos.append(getGridCenter(nx, ny));
        }
    }
}

void Widget::stop(){
    isAuto=false;
    isStarted = false;
    finished = true;
    ui->autoButton->setText("自动运行");
    ui->nextButton->setEnabled(false);
    ui->testButton->setEnabled(true);
    QApplication::processEvents(QEventLoop::AllEvents | QEventLoop::ExcludeUserInputEvents, 50);
    // 清空绘制容器，避免销毁时绘制冲突
    curKnightPos = QPoint();
    visitedPos.clear();
    validPos.clear();
    drawFrame->update();
}

void Widget::adjustLabel(){
    QString width="0";
    for (int i=1;i<m;i++){
        width=QString(width+"         %1").arg(i);
    }
    QString height="5";
    for (int i=6;i<n;i++){
        height=QString(height+"\n\n%1").arg(i);
    }
    for (int i=n;i<10;i++){
        height=QString(height+"\n\n");
    }
    ui->heightLabel_2->setText(height);
    ui->widthLabel->setText(width);
}

void Widget::on_speedBox_currentIndexChanged(int index)
{
    index = ui->speedBox->currentIndex();
    if (index==0){
        INTERVAL=MIDDLEINTERVAL;
    }else if (index==1){
        INTERVAL=SLOWINTERVAL;
    }else{
        INTERVAL=FASTINTERVAL;
    }
    ui->nextButton->setFocus();
}

