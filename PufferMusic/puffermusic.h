#ifndef PUFFERMUSIC_H
#define PUFFERMUSIC_H
#include <QJsonArray>
#include <QWidget>
#include <volumetool.h>
#include "musiclist.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "commonpage.h"
#include "lrcpage.h"
#include <QPropertyAnimation>
#include <QSqlDatabase>
QT_BEGIN_NAMESPACE
namespace Ui { class PufferMusic; }
QT_END_NAMESPACE

class PufferMusic : public QWidget
{
    Q_OBJECT

public:
    PufferMusic(QWidget *parent = nullptr);
    ~PufferMusic();
    void initUi();
    void initPlayer();
    void connectSignAndSlots();
    QJsonArray randomPiction();
    void initSQList();
    void initMusicList();

    void upDateMusicAnimal();
    void onPuferMusicExit();

private slots:
    void on_exit_clicked();
    void onBtFromClick(int pageId);


    void on_volume_clicked();

    void on_add_local_clicked();

    void on_lrcWord_clicked();

    void updateMusicLikeAndPage(bool isLike,const QString& musicId);

    void onPlayMusic();

    void onPlayStateChanged();

    void on_playMusicUp();
    void on_playMusicDown();
    void on_playModeCheng();
    void Icon_playModeCheng(QMediaPlaylist::PlaybackMode playbackmoe);
    void on_playALl(MusicListType musicPageType);

    void on_playIndexCheng(int index);


    void playAllMusicOfCommonPage(commonPage* page,int index);
    void playMusicByIndex(commonPage* page,int index);

    //静音、滑块调节音量
    void setMusicMeted(bool meted);
    void setMusicVolume(int volume);

    //歌曲总时长改变信号
    void onDurationCheng(qint64 druation);
    //歌曲现在时长改变信号
    void onPositionChanged(qint64 position);
    //进度条改变音乐point 时长
    void onSilderChengMusic(float ratio);
    //元数据发生改变时 进行获取
    void onMetaDataAvailableChanged(bool available);




    void on_background_color_clicked();

    void on_min_clicked();

    void on_max_clicked();

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    Ui::PufferMusic *ui;
    QPoint dragPosition;
    volumeTool* volumetool;
    QSqlDatabase sqlList;
    MusicList musicList;
    QMediaPlayer* player;
    QMediaPlaylist* playlist;
    lrcPage* lrcpage;
    QPropertyAnimation* lrcPageAnimal;

    commonPage* PlayingPage;
    qint64 totaltime;
    int currentPlayMusicIndex;
    bool isdrag;
};
#endif // PUFFERMUSIC_H
