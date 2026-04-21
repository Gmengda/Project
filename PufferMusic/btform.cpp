#include "btform.h"
#include "ui_btform.h"
#include <QPropertyAnimation>

BtForm::BtForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BtForm)
{
    ui->setupUi(this);
    ui->move_line->hide();
    //设置line1动画效果
    animal1 = new QPropertyAnimation(ui->line1,"geometry",this);
    animal1->setDuration(1500);
    animal1->setKeyValueAt(0,QRect(4,15,2,0));
    animal1->setKeyValueAt(0.5,QRect(4,0,2,15));
    animal1->setKeyValueAt(1,QRect(4,15,2,0));
    animal1->setLoopCount(-1);
    animal1->start();
    //设置line2动画效果
    animal2 = new QPropertyAnimation(ui->line2,"geometry",this);
    animal2->setDuration(1400);
    animal2->setKeyValueAt(0,QRect(20,15,2,0));
    animal2->setKeyValueAt(0.5,QRect(20,0,2,15));
    animal2->setKeyValueAt(1,QRect(20,15,2,0));
    animal2->setLoopCount(-1);
    animal2->start();
    //设置line3动画效果
    animal3 = new QPropertyAnimation(ui->line3,"geometry",this);
    animal3->setDuration(1600);
    animal3->setKeyValueAt(0,QRect(12,15,2,0));
    animal3->setKeyValueAt(0.5,QRect(12,0,2,15));
    animal3->setKeyValueAt(1,QRect(12,15,2,0));
    animal3->setLoopCount(-1);
    animal3->start();
    //设置line4动画效果
    animal4 = new QPropertyAnimation(ui->line4,"geometry",this);
    animal4->setDuration(1800);
    animal4->setKeyValueAt(0,QRect(28,15,2,0));
    animal4->setKeyValueAt(0.5,QRect(28,0,2,15));
    animal4->setKeyValueAt(1,QRect(28,15,2,0));
    animal4->setLoopCount(-1);
    animal4->start();

}

BtForm::~BtForm()
{
    delete ui;
}

void BtForm::setButIconAndText(const QString &QIcon, const QString &QText,int pageIdClick )
{
    ui->bt_icon->setPixmap(QPixmap(QIcon));
    ui->bt_text->setText(QText);
   //将按钮和页面进行绑定
    pageId = pageIdClick;
}

int BtForm::getPageId() const
{
    return pageId;
}

void BtForm::clearBackground()
{
    ui->btStyle->setStyleSheet("#btStyle:hover{background-color:rgb(204, 204, 204);}");
}

void BtForm::showAnmial(bool isShow)
{
    if(isShow)
    {
        ui->move_line->show();
    }
    else
    {
        ui->move_line->hide();
    }

}

void BtForm::mousePressEvent(QMouseEvent *event)
{
    (void)event;
    ui->btStyle->setStyleSheet("#btStyle{background-color:rgb(30,206,154);}");

    emit btClick(pageId);
}
