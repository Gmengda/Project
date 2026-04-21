#include "commonpage.h"
#include "ui_commonpage.h"
#include <listitembox.h>
#include <QDebug>
#include "music.h"
#include "musiclist.h"
commonPage::commonPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::commonPage)
{
    ui->setupUi(this);
    ui->pageMusicList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(ui->playMusicBtn,&QPushButton::clicked,this,[=](){
        //通知pufferMusic播放该页面所有的歌曲
        emit playAll(musicListType);
    });
    connect(ui->pageMusicList,&QListWidget::doubleClicked,this,[=](const QModelIndex index){
        emit playMusicByIndex(this,index.row());
    });
}

commonPage::~commonPage()
{
    delete ui;
}

void commonPage::creatCommonPageUi(const QString& text,const QString& path)
{
    ui->pageText->setText(text);
    ui->musicImage->setPixmap(QPixmap(path));
    ui->musicImage->setScaledContents(true);
}

void commonPage::setMusicListType(MusicListType musicListType)
{
    this->musicListType = musicListType;
}

void commonPage::addMusicToPage(MusicList& musicList)
{
    ////先清除数据，再清除界面上的
    musicOfPage.clear();


    //添加一个begin end
    for(auto e : musicList)
    {
        switch(musicListType)
        {
        case LIKE_LIST:
            if(e.getMusicLike())
            {
                musicOfPage.push_back(e.getMusicUUID());
            }
            break;
        case LOCAL_LIST:
            musicOfPage.push_back(e.getMusicUUID());
            break;
        case HISTORY_LIST:
            if(e.getMusicHistory())
            {
                musicOfPage.push_back(e.getMusicUUID());
            }
            break;
        default:
            qDebug()<<"暂未支持";
        }
    }
}

void commonPage::reFrsh(MusicList &musicList)
{
    ////先清除数据，再清除界面上
    ui->pageMusicList->clear();
    addMusicToPage(musicList);
    for(auto e : musicOfPage)
    {
        auto it = musicList.findMusicById(e);
        if(it == musicList.end())
        {
            continue;
        }
        //插入listitembox进行测试
        listItemBox* item = new listItemBox(this);
        //设置歌曲信息
        item->setMusicName(it->getMusicName());
        item->setMusicSinger(it->getMusicSinger());
        item->setMusicAlome(it->getMusicAlome());
        item->setIsLike(it->getMusicLike());

        QListWidgetItem* widgetItem = new QListWidgetItem(ui->pageMusicList);
        widgetItem->setSizeHint(QSize(item->width(),item->height()));
        ui->pageMusicList->setItemWidget(widgetItem,item);

        connect(item,&listItemBox::setMusicIsLike,this,[=](bool isLike){
            //更新歌曲状态

            //起到中转的作用
            //通知父节点 修改其他两个界面的状态
            emit updateMusicLike(isLike,it->getMusicUUID());
        });

    }

    //窗口重绘
    //update();  //窗口重绘放在循环队列中，没有立马处理
    repaint(); //立马相应paineEvent实践


}

void commonPage::addMusicToPlaylist(MusicList &musicList, QMediaPlaylist* playList)
{
    qDebug() << "当前页面类型：" << musicListType;
    playList->clear();
    for(auto& e : musicList)
    {
        switch(musicListType)
        {
        case LIKE_LIST:
            if(e.getMusicLike())
            {
                playList->addMedia(e.getMusicQUrl());
            }
            break;
        case LOCAL_LIST:
            playList->addMedia(e.getMusicQUrl());
            break;
        case HISTORY_LIST:
            if(e.getMusicHistory())
            {
                playList->addMedia(e.getMusicQUrl());
            }
            break;
        default:
            qDebug()<<"暂未支持";

        }
    }
}

QString commonPage::findMusicOfIndex(int index)
{
    return musicOfPage[index];
}

void commonPage::setMusicImage(QPixmap pixmap)
{
    ui->musicImage->setPixmap(pixmap);
    ui->musicImage->setScaledContents(true);
}















