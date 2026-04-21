#include "puffermusic.h"
#include "ui_puffermusic.h"
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QFileDialog>
#include "musiclist.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QColorDialog>
#include "btform.h"
#include <QSystemTrayIcon>
#include <QMenu>
PufferMusic::PufferMusic(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PufferMusic)
    , currentPlayMusicIndex(-1)
{
    ui->setupUi(this);
    initSQList();
    initUi();
    initPlayer();
    initMusicList();
    connectSignAndSlots();
        PlayingPage->addMusicToPlaylist(musicList,playlist);
}

PufferMusic::~PufferMusic()
{
    delete ui;
}

void PufferMusic::initUi()
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowIcon(QIcon(":/image/ICON.png"));

    //添加系统托盘
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/image/ICON.png"));
    trayIcon->show();

    //托盘菜单
    QMenu* trayMenu = new QMenu();
    trayMenu->addAction("显示",this,&QWidget::showNormal);
    trayMenu->addAction("关闭",this,&PufferMusic::onPuferMusicExit);
    trayIcon->setContextMenu(trayMenu);

    //窗口背景透明、窗口阴影
    this->setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect* shadoweffect = new QGraphicsDropShadowEffect(this);
    shadoweffect->setOffset(0,0);
    shadoweffect->setColor("#000000");
    shadoweffect->setBlurRadius(5);
    this->setGraphicsEffect(shadoweffect);
    ui->good->setButIconAndText(":/image/1 (11).png","推荐",1);
    ui->rec->setButIconAndText(":/image/1 (4).png","电台",0);
    ui->music->setButIconAndText(":/image/1 (4).png","音乐馆",2);
    ui->like->setButIconAndText(":/image/1 (12).png","我的喜欢",3);
    ui->local->setButIconAndText(":/image/1 (13).png","本地下载",4);
    ui->recent->setButIconAndText(":/image/1 (16).png","历史收听",5);
    //动画设置
    ui->local->showAnmial(true);
    PlayingPage = ui->local_page;
    ui->stackedWidget->setCurrentIndex(4);
    //打乱所以图片
    srand(time(NULL));
    //推荐的设置和图片呈现
    ui->recMusicBox->initRecBoxUi(randomPiction(),1);
    ui->muxicCocoBox->initRecBoxUi(randomPiction(),2);
    //设置commonPage信息
    ui->like_page->creatCommonPageUi("我的喜欢",":/image/icon/1 (16).jpg");

    ui->local_page->creatCommonPageUi("本地下载",":/image/icon/1 (17).jpg");

    ui->recent_page->creatCommonPageUi("最近收听",":/image/icon/1 (18).jpg");


    volumetool = new volumeTool();

    ui->play->setIcon(QIcon(":/image/bf.png"));
    ui->play_mode->setIcon(QIcon(":/image/1 (20).png"));

    //歌词初始化
    lrcpage = new lrcPage(this);
    lrcpage->setGeometry(10,10,lrcpage->width(),lrcpage->height());
    lrcpage->hide();
    //初始化上移动画对象
    lrcPageAnimal = new QPropertyAnimation(lrcpage,"geometry",this);
    lrcPageAnimal->setDuration(250);
    lrcPageAnimal->setStartValue(QRect(0,0 + lrcpage->height(),lrcpage->width(),lrcpage->height()));
    lrcPageAnimal->setEndValue(QRect(0,0,lrcpage->width(),lrcpage->height()));

}

void PufferMusic::initPlayer()
{
    //1相关类型初始化
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    //2设置基础模式
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    //3将play设置到playlist当中
    player->setPlaylist(playlist);
    player->setVolume(50);

    connect(ui->play,&QPushButton::clicked,this,&PufferMusic::onPlayMusic);
    connect(player,&QMediaPlayer::stateChanged,this,&PufferMusic::onPlayStateChanged);
    connect(ui->play_up,&QPushButton::clicked,this,&PufferMusic::on_playMusicUp);
    connect(ui->play_down,&QPushButton::clicked,this,&PufferMusic::on_playMusicDown);
    connect(ui->play_mode,&QPushButton::clicked,this,&PufferMusic::on_playModeCheng);
    connect(playlist,&QMediaPlaylist::playbackModeChanged,this,&PufferMusic::Icon_playModeCheng);

    //播放所有音乐
    connect(ui->local_page,&commonPage::playAll,this,&PufferMusic::on_playALl);
    connect(ui->like_page,&commonPage::playAll,this,&PufferMusic::on_playALl);
    connect(ui->recent_page,&commonPage::playAll,this,&PufferMusic::on_playALl);
    //双击播放音乐
    connect(ui->local_page,&commonPage::playMusicByIndex,this,&PufferMusic::playMusicByIndex);
    connect(ui->like_page,&commonPage::playMusicByIndex,this,&PufferMusic::playMusicByIndex);
    connect(ui->recent_page,&commonPage::playMusicByIndex,this,&PufferMusic::playMusicByIndex);

    //playList播放源发生变化时
    connect(playlist,&QMediaPlaylist::currentIndexChanged,this,&PufferMusic::on_playIndexCheng);

    //静音按钮
    connect(volumetool,&volumeTool::setMusicMuted,this,&PufferMusic::setMusicMeted);
    //滑块改变音量
    connect(volumetool,&volumeTool::setMusicVolume,this,&PufferMusic::setMusicVolume);
    //播放源时长改变
    connect(player,&QMediaPlayer::durationChanged,this,&PufferMusic::onDurationCheng);
    connect(player,&QMediaPlayer::positionChanged,this,&PufferMusic::onPositionChanged);
    //源文件改变
    connect(player,&QMediaPlayer::metaDataAvailableChanged,this,&PufferMusic::onMetaDataAvailableChanged);

}

void PufferMusic::connectSignAndSlots()
{

    connect(ui->good,&BtForm::btClick,this,&PufferMusic::onBtFromClick);
    connect(ui->rec,&BtForm::btClick,this,&PufferMusic::onBtFromClick);
    connect(ui->music,&BtForm::btClick,this,&PufferMusic::onBtFromClick);
    connect(ui->like,&BtForm::btClick,this,&PufferMusic::onBtFromClick);
    connect(ui->local,&BtForm::btClick,this,&PufferMusic::onBtFromClick);
    connect(ui->recent,&BtForm::btClick,this,&PufferMusic::onBtFromClick);
    connect(ui->music_word,&QPushButton::clicked,this,&PufferMusic::on_lrcWord_clicked);

    connect(ui->local_page,&commonPage::updateMusicLike,this,&PufferMusic::updateMusicLikeAndPage);
    connect(ui->like_page,&commonPage::updateMusicLike,this,&PufferMusic::updateMusicLikeAndPage);
    connect(ui->recent_page,&commonPage::updateMusicLike,this,&PufferMusic::updateMusicLikeAndPage);

    connect(ui->progress_bar,&musicSilder::musicPosition,this,&PufferMusic::onSilderChengMusic);

}

QJsonArray PufferMusic::randomPiction()
{
    QVector<QString> vecImageName;
    vecImageName<<"1 (1).jpg"<<"1 (2).jpg"<<"1 (3).jpg"<<"1 (4).jpg"<<"1 (5).jpg"
                <<"1 (6).jpg"<<"1 (7).jpg"<<"1 (8).jpg"<<"1 (9).jpg"<<"1 (10).jpg"
                <<"1 (11).jpg"<<"1 (12).jpg"<<"1 (13).jpg"<<"1 (14).jpg"<<"1 (15).jpg"
                <<"1 (16).jpg"<<"1 (17).jpg"<<"1 (18).jpg"<<"1 (19).jpg"<<"1 (20).jpg"
                <<"1 (21).jpg"<<"1 (22).jpg"<<"1 (23).jpg"<<"1 (24).jpg"<<"1 (25).jpg"<<"1 (26).jpg";
    std::random_shuffle(vecImageName.begin(),vecImageName.end());
    QJsonArray objArray;
    for(int i = 0 ; i < vecImageName.size() ; i++ )
    {
        QJsonObject Jsobject;
        Jsobject.insert("path",":/image/icon/" + vecImageName[i]);
        QString strText = QString("推荐-%1").arg(i,3,10,QChar('0'));
        Jsobject.insert("text",strText);
        objArray.append(Jsobject);
    }

    return objArray;
}

void PufferMusic::initSQList()
{
    //加载数据库
    QSqlDatabase sqllist = QSqlDatabase::addDatabase("QSQLITE");
    //设置数据库名字
    sqllist.setDatabaseName("PufferMusic.db");
    //打开数据库
    if(!sqllist.open())
    {
        QMessageBox::critical(this,"PufferMusic","数据库打开失败");
    }
    //创建表
    QString sql = "CREATE TABLE IF NOT EXISTS MusicInfo(\
                    id INTEGER PRIMARY KEY AUTOINCREMENT,\
                    musicId varchar(50) UNIQUE,\
                    musicName varchar(50),\
                    musicSinger varchar(50),\
                    musicAlome varchar(50),\
                    musicTime BIGINT,\
                    musicUrl varchar(256),\
                    isLike INTEGER,\
                    isHistory INTEGER)";
    QSqlQuery query;
    if(!query.exec(sql))
    {
        QMessageBox::critical(this,"PufferMusic","数据初始化错误");
    }

}

void PufferMusic::initMusicList()
{
    musicList.readMusicListToMpufferSql();
    ui->recent_page->setMusicListType(MusicListType::HISTORY_LIST);
    ui->recent_page->reFrsh(musicList);


    ui->local_page->setMusicListType(MusicListType::LOCAL_LIST);
    ui->local_page->reFrsh(musicList);

    ui->like_page->setMusicListType(MusicListType::LIKE_LIST);
    ui->like_page->reFrsh(musicList);



}

void PufferMusic::upDateMusicAnimal()
{
    int index = ui->stackedWidget->indexOf(PlayingPage);
    if(-1 == index)
    {
        return ;
    }
    QList<BtForm*> btform = this->findChildren<BtForm*>();
    for(auto e : btform)
    {
        if(e->getPageId() == index)
        {
            e->showAnmial(true);
        }
        else
        {
            e->showAnmial(false);
        }
    }
}

void PufferMusic::onPuferMusicExit()
{
    //歌曲信息写入数据库
    musicList.writeMusicListToMpufferSql();
    sqlList.close();
    this->close();
}

void PufferMusic::on_exit_clicked()
{
    hide();
}

void PufferMusic::onBtFromClick(int pageId)
{
    //获取所以BtFrom按钮
    QList<BtForm*> btFromList= this->findChildren<BtForm*>();
    for(auto btFrom : btFromList)
    {

        if(btFrom->getPageId() != pageId)
        {
            btFrom->clearBackground();
        }
    }

    isdrag = false;
    ui->stackedWidget->setCurrentIndex(pageId);
}

void PufferMusic::mouseMoveEvent(QMouseEvent *event)
{
    if(Qt::LeftButton == event->buttons() && isdrag)
    {
        move(event->globalPos()-dragPosition);
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void PufferMusic::mousePressEvent(QMouseEvent *event)
{
    if(Qt::LeftButton == event->button())
    {
        isdrag = true;
        dragPosition = event->globalPos() - geometry().topLeft();
        return;
    }
    QWidget::mousePressEvent(event);
}

void PufferMusic::on_volume_clicked()
{
    QPoint point = ui->volume->mapToGlobal(QPoint(0,0));
    QPoint volumeLeftTop = point - QPoint(volumetool->width()/2,volumetool->height());
    volumeLeftTop.setX(volumeLeftTop.x() + 15);
    volumetool->move(volumeLeftTop);
    volumetool->show();

}

void PufferMusic::on_add_local_clicked()
{
    //添加文件
    QFileDialog filedialog(this);

    filedialog.setWindowTitle("添加本地音乐");
    //控制打开文件的路径
    QDir dir = QDir::currentPath();
    dir.cdUp();
    QString projectPath = dir.path() + "/pufferMusic";
    filedialog.setDirectory(projectPath);

    //一次打开多个文件
    filedialog.setFileMode(QFileDialog::ExistingFiles);
    //通过文件后缀显示想要的文件
    //filedialog.setNameFilter("音乐文件(*.h *.c *.hpp *.cpp)");
    //通过MIME类型显示想要的文件
    QStringList mimeTypeFilters;
    mimeTypeFilters << "*";
    filedialog.setNameFilters(mimeTypeFilters);

    if(QDialog::Accepted == filedialog.exec())
    {
        //1将commonPage切换到本地下载
        ui->stackedWidget->setCurrentIndex(4);
        //获取选中的文件
        QList<QUrl> fileUrls = filedialog.selectedUrls();
        //fileUrls内部存放的是选中文件的路径
        //将音乐添加到MusicList中
        musicList.addMusicsByUrl(fileUrls);

        //2将歌曲信息更新到commonPage页面
        ui->local_page->reFrsh(musicList);
        ui->local_page->addMusicToPlaylist(musicList,playlist);

    }

}

void PufferMusic::on_lrcWord_clicked()
{
//    lrcPageAnimal->start();
    lrcpage->show();

}

void PufferMusic::updateMusicLikeAndPage(bool isLike, const QString &musicId)
{
    auto it = musicList.findMusicById(musicId);

    if(it != musicList.end())
    {
        it->setMusicLike(isLike);
    }
    ui->like_page->reFrsh(musicList);
    ui->like_page->addMusicToPlaylist(musicList,playlist);
    ui->local_page->reFrsh(musicList);
    ui->local_page->addMusicToPlaylist(musicList,playlist);
    ui->recent_page->reFrsh(musicList);
    ui->recent_page->addMusicToPlaylist(musicList,playlist);
}

//播放和暂停点击事件
void PufferMusic::onPlayMusic()
{
    if(QMediaPlayer::PlayingState == player->state())
    {
        //ui->play->setIcon(QIcon(":/image/bf.png"));
        player->pause();
    }
    else if(QMediaPlayer::PausedState == player->state())
    {
        //ui->play->setIcon(QIcon(":/image/puse.png"));
        player->play();
    }
    else if(QMediaPlayer::StoppedState == player->state())
    {
        //ui->play->setIcon(QIcon(":/image/puse.png"));
        player->play();
    }
    else
    {
        qDebug() << player->errorString();
    }
}
//图片切换
void PufferMusic::onPlayStateChanged()
{
    if(QMediaPlayer::PlayingState == player->state())
    {
        ui->play->setIcon(QIcon(":/image/puse.png"));

    }
    else
    {
        ui->play->setIcon(QIcon(":/image/bf.png"));
    }
}

void PufferMusic::on_playMusicUp()
{
    playlist->previous();
}

void PufferMusic::on_playMusicDown()
{
    playlist->next();
}

void PufferMusic::on_playModeCheng()
{
    if(QMediaPlaylist::Loop == playlist->playbackMode())
    {
        playlist->setPlaybackMode(QMediaPlaylist::Random);
        ui->play_mode->setToolTip("随机播放");
    }
    else if(QMediaPlaylist::Random == playlist->playbackMode())
    {
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        ui->play_mode->setToolTip("单曲播放");
    }
    else if(QMediaPlaylist::CurrentItemInLoop == playlist->playbackMode())
    {
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        ui->play_mode->setToolTip("顺序播放");
    }
}

void PufferMusic::Icon_playModeCheng(QMediaPlaylist::PlaybackMode playbackmoe)
{
    if(QMediaPlaylist::Loop == playbackmoe)
    {
        ui->play_mode->setIcon(QIcon(":/image/1 (20).png"));
    }
    else if(QMediaPlaylist::Random == playbackmoe)
    {
        ui->play_mode->setIcon(QIcon(":/image/1 (10).png"));
    }
    else if(QMediaPlaylist::CurrentItemInLoop == playbackmoe)
    {
        ui->play_mode->setIcon(QIcon(":/image/1 (3).png"));
    }
}

void PufferMusic::on_playALl(MusicListType musicPageType)
{
    commonPage* page;
    switch(musicPageType)
    {
    case LIKE_LIST:
        page = ui->like_page;
        break;
    case LOCAL_LIST:
        page = ui->local_page;
        break;
    case HISTORY_LIST:
        page = ui->recent_page;
        break;
    default:
        qDebug() << "没有此页面";
    }

    playAllMusicOfCommonPage(page,0);

}

void PufferMusic::on_playIndexCheng(int index)
{
    currentPlayMusicIndex = index;
    //由于playlist中的先后次序和musiclist是相同的
    //直接将index找到放在playlist中即可
    QString musicid = PlayingPage->findMusicOfIndex(index);
    auto it = musicList.findMusicById(musicid);
    if(it != musicList.end())
    {
        it->setMusicHistory(true);
    }
    ui->recent_page->reFrsh(musicList);
}

void PufferMusic::playAllMusicOfCommonPage(commonPage *page, int index)
{
    PlayingPage = page;
    this->upDateMusicAnimal();
    playlist->clear();
    page->addMusicToPlaylist(musicList,playlist);
    playlist->setCurrentIndex(index);
    player->play();

}

void PufferMusic::playMusicByIndex(commonPage *page, int index)
{
    playAllMusicOfCommonPage(page,index);
}

void PufferMusic::setMusicMeted(bool meted)
{
    player->setMuted(meted);
}

void PufferMusic::setMusicVolume(int volume)
{
    player->setVolume(volume);
}

void PufferMusic::onDurationCheng(qint64 druation)
{
    //总时长转化为分，秒
    ui->all_time->setText(QString("%1 : %2").arg(druation/1000/60,2,10,QChar('0')).arg(druation/1000%60,2,10,QChar('0')));
    totaltime = druation;
}

void PufferMusic::onPositionChanged(qint64 position)
{
    ui->progress_bar->silderScoendLoop(position/(float)totaltime);
    ui->now_time->setText(QString("%1 : %2").arg(position/1000/60,2,10,QChar('0')).arg(position/1000%60,2,10,QChar('0')));
    //同步lrc中的歌词信息
    if(currentPlayMusicIndex>=0)
    {
        lrcpage->showLrcWord(position);
    }
}

void PufferMusic::onSilderChengMusic(float ratio)
{
    qint64 time = totaltime*ratio;
    ui->now_time->setText(QString("%1 : %2").arg(time/1000/60,2,10,QChar('0')).arg(time/1000%60,2,10,QChar('0')));
    player->setPosition(time);
}

void PufferMusic::onMetaDataAvailableChanged(bool available)
{
    (void)available;
    //歌曲名称  歌曲作者 直接到music对象中获取
    //此时需要知道现在播放歌曲的索引
    QString itid = PlayingPage->findMusicOfIndex(currentPlayMusicIndex);
    auto it = musicList.findMusicById(itid);
    QString musicName("未知歌曲");
    QString musicSinger("未知歌手");
    if(it != musicList.end())
    {
        musicName = it->getMusicName();
        musicSinger = it->getMusicSinger();
    }
    ui->music_name->setText(musicName);
    ui->music_linder->setText(musicSinger);
    //获取封面图
    QVariant coverimage = player->metaData("ThumbnailImage");
    if(coverimage.isValid())
    {
        QImage image = coverimage.value<QImage>();
        ui->music_image->setPixmap(QPixmap::fromImage(image));
        ui->music_image->setScaledContents(true);
        PlayingPage->setMusicImage(QPixmap::fromImage(image));
    }
    else
    {
        ui->music_image->setPixmap(QString(":/image/icon/1 (14).jpg"));
        ui->music_image->setScaledContents(true);
        PlayingPage->setMusicImage(QString(":/image/icon/1 (14).jpg"));
    }

    //解析歌曲的lrc歌词

    if(it != musicList.end())
    {
        QString music = it->getMusicUrl();
        lrcpage->parseLrcFile(music);
    }

}


void PufferMusic::on_background_color_clicked()
{
    QColor color = QColorDialog::getColor(Qt::Widget,this);
    if(!color.isValid())
    {
        QMessageBox::information(this,"提示","选择错误");
    }
    ui->head->setStyleSheet(QString("#head {background-color:rgb(%1, %2, %3);}").arg(color.red()).arg(color.green()).arg(color.blue()));
    ui->body_left->setStyleSheet(QString("#body_left {background-color:rgb(%1, %2, %3);}").arg(color.red()).arg(color.green()).arg(color.blue()));
    update();
    repaint();
    for (QWidget *w : findChildren<QWidget*>()) {
        w->update();
    }
}

void PufferMusic::on_min_clicked()
{
    this->showMinimized();
}

void PufferMusic::on_max_clicked()
{
    QMessageBox::information(this,"提示","暂未设计");
}
