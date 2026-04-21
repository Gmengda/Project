#ifndef COMMONPAGE_H
#define COMMONPAGE_H
#include"musiclist.h"
#include <QWidget>
#include <QMediaPlaylist>
namespace Ui {
class commonPage;
}

//添加枚举区分界面
enum MusicListType
{
    LIKE_LIST,
    LOCAL_LIST,
    HISTORY_LIST
};

class commonPage : public QWidget
{
    Q_OBJECT

public:
    explicit commonPage(QWidget *parent = nullptr);
    ~commonPage();
    void creatCommonPageUi(const QString& text,const QString& path);
    void setMusicListType(MusicListType musicListType);
    void addMusicToPage(MusicList& musicList);
    void reFrsh(MusicList& musicList);

    //将歌曲添加到播放列表当中
    void addMusicToPlaylist(MusicList& musicList , QMediaPlaylist* playList);

    //通过index找到歌曲
    QString findMusicOfIndex(int index);

    void setMusicImage(QPixmap pixmap);
signals:
    void updateMusicLike(bool isLike,const QString& musicId);
    void playAll(MusicListType musicListType);

    void playMusicByIndex(commonPage* page, int index);

private:
    Ui::commonPage *ui;
    MusicListType musicListType;   //页面类别
    QVector<QString>  musicOfPage; //保存页面对应歌曲的id
};

#endif // COMMONPAGE_H
