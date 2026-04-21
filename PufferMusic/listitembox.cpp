#include "listitembox.h"
#include "ui_listitembox.h"
#include <QDebug>
listItemBox::listItemBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::listItemBox)
  ,isLike(false)
{
    ui->setupUi(this);
    connect(ui->pushButton,&QPushButton::clicked,this,&listItemBox::onBtnClick);
}

listItemBox::~listItemBox()
{
    delete ui;
}

void listItemBox::enterEvent(QEvent *event)
{
    (void)event;
    setStyleSheet("background-color:#EFEFEF;");
}

void listItemBox::leaveEvent(QEvent *event)
{
    (void)event;
    setStyleSheet("");
}
void listItemBox::setMusicName(const QString &musicname)
{
    ui->musicNameLabel->setText(musicname);
}

void listItemBox::setMusicSinger(const QString &musicsinger)
{
    ui->musicSingerLabel->setText(musicsinger);
}

void listItemBox::setMusicAlome(const QString &musicalome)
{
    ui->musciAlbumLabel->setText(musicalome);
}

void listItemBox::setIsLike(bool islike)
{
    this->isLike = islike;
    if(this->isLike)
    {
        ui->pushButton->setIcon(QIcon(":/image/like (1).png"));
    }
    else
    {
        ui->pushButton->setIcon(QIcon(":/image/like (2).png"));
    }
}

void listItemBox::onBtnClick()
{
    isLike = !isLike;
    setIsLike(isLike);

    //发射一个信号用于commonpage接受 , 用于修改Music中的Ｌｉｋｅ
    emit setMusicIsLike(isLike);

}


