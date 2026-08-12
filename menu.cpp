#include "menu.h"
#include "ui_menu.h"
#include <QScreen>
#include <QMessageBox>
#include <QThread>

Menu::Menu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Menu)
    , m_widget(nullptr)
{
    ui->setupUi(this);
    ui->suggestion_3->setReadOnly(true);
    ui->title->setReadOnly(true);
    ui->suggestion->setReadOnly(true);
    ui->heightEdit->setAlignment(Qt::AlignCenter);
    ui->widthEdit->setAlignment(Qt::AlignCenter);
    this->setWindowTitle("马的遍历问题");
    this->setFixedSize(400, 300);
    m_table=nullptr;

    setWindowIcon(QIcon(":/picture.ico"));
}

Menu::~Menu()
{
    delete ui;
    delete m_widget;
    delete m_table;
}


void Menu::on_completeButton_clicked()
{
    if (m_widget != nullptr) {
        m_widget->stop();
        m_widget->safelyExit();
        QApplication::processEvents(QEventLoop::AllEvents, 50);
        //delete m_widget;
        //m_widget = nullptr;
    }
    if (m_table) delete m_table;
    m_table=new class table();
    setPosition();
    m_widget->setWindowTitle("马的遍历问题-标准棋盘");
    showWidget();
}


void Menu::setPosition(){
    m_widget = new Widget(nullptr, m_table);
    //强制设置固定尺寸，覆盖Widget自身UI的尺寸限制
    m_widget->setMinimumSize(1080, 720);
    m_widget->setMaximumSize(1080, 720);
    m_widget->resize(1080, 720); // 强制调整
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();
    QRect windowRect = m_widget->frameGeometry();

    // 计算居中位置
    int x = (screenRect.width()  - windowRect.width())  / 2;
    int y = (screenRect.height()  - windowRect.height())  / 2-16;

    m_widget->move(x, y);
}


void Menu::showWidget(){
    // 每次点击都强制显示并前置
    m_widget->show();
    m_widget->raise();
    m_widget->activateWindow();
    //m_widget=nullptr;
}

void Menu::on_selfDefinedButton_clicked()
{
    QString height = ui->heightEdit->text();
    QString width = ui->widthEdit->text();
    int h=0,w=0;

    //特殊情况处理
    bool hasError=false;

    if (height.size()==1 && height[0]<='9' && height[0]>='6'){
        h=height.toInt();
    }else if (height=="10"){
        h=10;
    }else{
        hasError=true;
    }

    if (width.size()==1 && width[0]<='9' && width[0]>='6'){
        w=width.toInt();
    }else if (width=="10"){
        w=10;
    }else{
        hasError=true;
    }

    if (h==10 && w==10){
        hasError=true;
    }

    if (hasError){
        QMessageBox::warning(this, "提示", "请输入正确的棋盘尺寸\n（规格：6×6至9×10）");
        return;
    }

    if (h<w){
        int tmp=h;h=w;w=tmp;
    }

    if (m_widget != nullptr) {
        m_widget->stop();
        m_widget->safelyExit();
        QApplication::processEvents(QEventLoop::AllEvents, 50);
        //delete m_widget;
        //m_widget = nullptr;
    }
    if (m_table) delete m_table;
    m_table=new table();
    m_table->setSize(h,w);
    setPosition();
    QString title=QString("马的遍历问题-自定义棋盘（规格：%1×%2）").arg(w).arg(h);
    m_widget->setWindowTitle(title);
    showWidget();
}

