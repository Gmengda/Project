#include "recbox.h"
#include "ui_recbox.h"
#include "recboxitem.h"
#include <QJsonObject>
RecBox::RecBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecBox),
    row(1),
    col(4)
{
    ui->setupUi(this);
}

RecBox::~RecBox()
{
    delete ui;
}

void RecBox::initRecBoxUi(QJsonArray data, int row)
{

    if(2 == row)
    {
        this->row = row;
        this->col = 8;
    }
    else
    {
        ui->recMusicDown->hide();
    }
    imageList = data;

    currentIndex  = 0;
    count = imageList.size()/col;

    creatRecBoxItem();
}

void RecBox::creatRecBoxItem()
{
    QList<RecBoxItem*> uplist = ui->recMusicUp->findChildren<RecBoxItem*>();
    for(auto e : uplist)
    {
        ui->h_recMusicUp_1->removeWidget(e);
        delete e;
    }
    QList<RecBoxItem*> udownlist = ui->recMusicDown->findChildren<RecBoxItem*>();
    for(auto e : udownlist)
    {
        ui->h_recMusicDown_2->removeWidget(e);
        delete e;
    }
    int index = 0;
    for(int i = currentIndex*col ; i < col*(currentIndex+1) ; i++)
    {
        RecBoxItem* item = new RecBoxItem();
        item->setRecText((imageList[i].toObject()).value("text").toString());
        item->setRecImage((imageList[i].toObject()).value("path").toString());
        if(index >= col/2 && 2 == row)
        {
            ui->h_recMusicDown_2->addWidget(item);
        }
        else
        {
            ui->h_recMusicUp_1->addWidget(item);
        }
        index++;
    }
}

void RecBox::on_btnDown_clicked()
{
    currentIndex++;
    if(currentIndex >= count)
    {
       currentIndex  = 0;

    }
    creatRecBoxItem();

}

void RecBox::on_btnUp_clicked()
{
    currentIndex--;
    if(currentIndex < 0)
    {
       currentIndex  = count - 1;

    }
    creatRecBoxItem();
}
