#ifndef MUSICLIST_H
#define MUSICLIST_H
#include <QVector>
#include "music.h"
#include <QUrl>
#include <QList>
#include <QSet>

class MusicList
{
public:
    MusicList();
    void addMusicsByUrl(const QList<QUrl> url);
    QVector<Music>::iterator begin();
    QVector<Music>::iterator end();
    QVector<Music>::iterator findMusicById(const QString& MusicId);

    void writeMusicListToMpufferSql();
    void readMusicListToMpufferSql();
private:
    QVector<Music> musicList;
    QSet<QString> musicPath;  //防止重复加载
};

#endif // MUSICLIST_H
