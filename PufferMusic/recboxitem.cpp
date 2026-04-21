#include "recboxitem.h"
#include "ui_recboxitem.h"

RecBoxItem::RecBoxItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecBoxItem)
{
    ui->setupUi(this);
    ui->musci_image_btn->installEventFilter(this);
}

RecBoxItem::~RecBoxItem()
{
    delete ui;
}

bool RecBoxItem::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->musci_image_btn)
    {
        if(QEvent::Enter == event->type())
        {
            //添加图片移动动画
            QPropertyAnimation* animal = new QPropertyAnimation(ui->musci_image_btn,"geometry");
            animal->setDuration(150);
            animal->setStartValue(QRect(9,9,ui->rec_music_background->width(),ui->rec_music_background->height()));
            animal->setEndValue(QRect(9,0,ui->rec_music_background->width(),ui->rec_music_background->height()));
            animal->start();
            //处理animal
            connect(animal,&QPropertyAnimation::finished,this,[=](){
                delete animal;
            });
        }
        else if(QEvent::Leave == event->type())
        {
            QPropertyAnimation* animal = new QPropertyAnimation(ui->musci_image_btn,"geometry");
            animal->setDuration(150);
            animal->setStartValue(QRect(9,0,ui->rec_music_background->width(),ui->rec_music_background->height()));
            animal->setEndValue(QRect(9,9,ui->rec_music_background->width(),ui->rec_music_background->height()));
            animal->start();
            connect(animal,&QPropertyAnimation::finished,this,[=](){
                delete animal;
            });
        }
    }
    return QObject::eventFilter(watched,event);
}

void RecBoxItem::setRecText(const QString &text)
{
    ui->rec_text->setText(text);
}

void RecBoxItem::setRecImage(const QString &imageurl)
{
    QString style = "background-image:url("+imageurl+");";
    ui->rec_music_background->setStyleSheet(style);
}
