#ifndef MUSIC_H
#define MUSIC_H
#include <QUrl>

class Music
{
public:
    Music();
    Music(QUrl url);
    void setMusicName(const QString& musicname);
    void setMusicSinger(const QString& musicsinger);
    void setMusicAlome(const QString& musicalome);
    void setMusicTime(const qint64& musictime);
    void setMusicQUrl( QUrl musicurl);
    void setMusicLike( bool musiclike);
    void setMusicHistory( bool musichistory);
    void setMusicId(const QString& musicid);

    QString getMusicName()const;
    QString getMusicSinger()const;
    QString getMusicAlome()const;
    qint64 getMusicTime()const;
    QUrl getMusicQUrl()const;
    bool getMusicLike()const;
    bool getMusicHistory()const;
    QString getMusicUUID()const;

    QString getMusicUrl()const;

    void InsertMusicDB();
private:
    void parseMediaMetaMusic();
private:
    QString musicName;
    QString musicSinger;
    QString musicAlome;
    qint64 musicTime;
    QUrl musicUrl;
    bool isLike;
    bool isHistory;

    QString musicId;
};

#endif // MUSIC_H
