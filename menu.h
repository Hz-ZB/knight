#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include "widget.h"

namespace Ui {
class Menu;
}

class Menu : public QWidget
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    ~Menu();

private slots:
    void on_completeButton_clicked();

    void on_selfDefinedButton_clicked();

private:
    Ui::Menu *ui;
    Widget *m_widget;//子页面存储
    table *m_table;//子页面尺寸信息存储

    void setPosition();//设置子页面位置并移动
    void showWidget();//弹出子页面
};

#endif // MENU_H
