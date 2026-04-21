#include "musicsilder.h"
#include "ui_musicsilder.h"
#include <QMouseEvent>
#include <QDebug>
musicSilder::musicSilder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::musicSilder)
{
    ui->setupUi(this);
    maxWidth = width();
}

musicSilder::~musicSilder()
{
    delete ui;
}

void musicSilder::silderScoendLoop(float ratio)
{
    position = maxWidth*ratio;
    onSilderChing();
}

void musicSilder::mouseMoveEvent(QMouseEvent *event)
{
    QRect silderRect = QRect(0,0,geometry().width(),geometry().height());
    if(!silderRect.contains(event->pos()))
    {
        return;
    }
    if(Qt::LeftButton == event->buttons())
    {
        position = event->pos().x();
        if(position<0)
        {
            position = 0;
        }
        else if(position > maxWidth)
        {
            position = maxWidth;
        }
    }
    onSilderChing();
}

void musicSilder::mousePressEvent(QMouseEvent *event)
{
    position = event->pos().x();
    onSilderChing();
}

void musicSilder::mouseReleaseEvent(QMouseEvent *event)
{
    position = event->pos().x();
    onSilderChing();



    emit musicPosition(ui->outLine->width()/(float)maxWidth);
}

void musicSilder::onSilderChing()
{
    ui->outLine->setGeometry(ui->outLine->x(),ui->outLine->y(),position,ui->outLine->height());
}

