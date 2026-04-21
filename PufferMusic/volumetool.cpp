#include "volumetool.h"
#include "ui_volumetool.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QDebug>
volumeTool::volumeTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::volumeTool),
    isMuted(false),
    volue(50)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    setAttribute(Qt::WA_TranslucentBackground);

    //阴影效果
    QGraphicsDropShadowEffect* shadoweffect = new QGraphicsDropShadowEffect(this);
    shadoweffect->setOffset(0,0);
    shadoweffect->setColor("#646464");
    shadoweffect->setBlurRadius(10);
    this->setGraphicsEffect(shadoweffect);

    ui->pushButton->setIcon(QIcon(":/image/1 (15)2.png"));
    ui->volumeRatio->setText("50%");

    connect(ui->pushButton,&QPushButton::clicked,this,&volumeTool::on_btnClick);

    ui->widget->installEventFilter(this);
}
volumeTool::~volumeTool()
{
    delete ui;
}

void volumeTool::paintEvent(QPaintEvent *event)
{
    (void)event;
    //绘制三角形
    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::white));

    QPolygon polygon;
    QPoint a(10,298+10);
    QPoint b(10+80,298+10);
    QPoint c(10+40,298+10+30);
    polygon.append(a);
    polygon.append(b);
    polygon.append(c);
    painter.drawPolygon(polygon);

}

void volumeTool::on_btnClick()
{
    isMuted = !isMuted;
    if(!isMuted)
    {
        ui->pushButton->setIcon(QIcon(":/image/1 (15)2.png"));
    }
    else
    {
        ui->pushButton->setIcon(QIcon(":/image/1 (15)1.png"));
    }

    emit setMusicMuted(isMuted);
}

bool volumeTool::eventFilter(QObject *watched, QEvent *event)
{
    if(ui->widget == watched)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            calcVolume();
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {

            emit setMusicVolume(volue);
        }
        else if(event->type() == QEvent::MouseMove)
        {

            calcVolume();
            emit setMusicVolume(volue);
        }
        return true;
    }
    return eventFilter(watched,event);
}

void volumeTool::calcVolume()
{

    //计算音量大小，更新界面，更新文本数据
    //获取鼠标点击时的y坐标
    int y = ui->widget->mapFromGlobal(QCursor().pos()).y();
    if(y < 20)
    {
        y = 20;
    }
    else if(y >220)
    {
        y = 220;
    }

    ui->inLine->setGeometry(ui->inLine->x(),y,ui->inLine->width(),220-y);
    ui->pushBtn->move(ui->pushBtn->x(),ui->inLine->y() - ui->pushBtn->height()/2);
    volue = (int)((220-ui->inLine->y())/(float)200*100);
    ui->volumeRatio->setText(QString::number(volue,10) + "%");
}
