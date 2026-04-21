#include "music.h"
#include <QUuid>
#include <QMediaPlayer>
#include <QDebug>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
Music::Music()
{

}

Music::Music(QUrl url)
    :isLike(false)
    ,isHistory(false)
    ,musicUrl(url)
{
    musicId = QUuid::createUuid().toString();
    //读取url中的文件地址信息
    //读取 名称 作者 专辑 时长
    parseMediaMetaMusic();

}

void Music::setMusicName(const QString &musicname)
{
    this->musicName = musicname;
}

void Music::setMusicSinger(const QString &musicsinger)
{
    this->musicSinger = musicsinger;
}

void Music::setMusicAlome(const QString &musicalome)
{
    this->musicAlome = musicalome;
}

void Music::setMusicTime(const qint64& musictime)
{
    this->musicTime = musictime;
}

void Music::setMusicQUrl(QUrl musicurl)
{
    this->musicUrl = musicurl;
}

void Music::setMusicLike(bool musiclike)
{
    this->isLike = musiclike;
}

void Music::setMusicHistory(bool musichistory)
{
    this->isHistory = musichistory;
}

void Music::setMusicId(const QString &musicid)
{
    this->musicId = musicid;
}


QString Music::getMusicName()const
{
    return this->musicName;
}
QString Music::getMusicSinger()const
{
return this->musicSinger;
}
QString Music::getMusicAlome()const
{
return this->musicAlome;
}
qint64 Music::getMusicTime()const
{
return this->musicTime;
}
QUrl Music::getMusicQUrl()const
{
return this->musicUrl;
}
bool Music::getMusicLike()const
{
return this->isLike;
}
bool Music::getMusicHistory()const
{
    return this->isHistory;
}

QString Music::getMusicUUID() const
{
    return this->musicId;
}

QString Music::getMusicUrl() const
{
    QString lrc = musicUrl.toLocalFile();
    lrc.replace(".mp3",".lrc");
    lrc.replace(".fla",".lrc");
    lrc.replace(".mpga",".lrc");
    return lrc;
}

void Music::InsertMusicDB()
{
    //判断歌曲是否存在
    QSqlQuery query;
    //query.prepare("SELECT * FROM MusicInfo WHERE musicId = ?"); //返回该列所有字段 不需要怎么多
    query.prepare("SELECT EXISTS (SELECT 1 FROM MusicInfo WHERE musicId = ?)");
    query.addBindValue(musicId);
    if(!query.exec())
    {
        qDebug() << "查询失败" << query.lastError();
        return ;
    }
    bool isexists = query.value(0).toBool();

    if(isexists)
    {
        //存在 更新like 和 history
        query.prepare("UPDATE MusicInfo SET isLike = ?,isHistory = ? WHERE musicId = ?");
        query.addBindValue(isLike ? 1 : 0);
        query.addBindValue(isHistory ? 1 : 0);
        query.addBindValue(musicId);
        if(!query.exec())
        {
            qDebug() << "更新失败" << query.lastError();
            return ;
        }
        qDebug() << "更新" << musicName;
    }
    else
    {
        //不存在 直接全加
        query.prepare("INSERT INTO MusicInfo(musicId,musicName,musicSinger,musicAlome,musicTime,musicUrl,isLike,isHistory) VALUES(?,?,?,?,?,?,?,?)");
        query.addBindValue(musicId);
        query.addBindValue(musicName);
        query.addBindValue(musicSinger);
        query.addBindValue(musicAlome);
        query.addBindValue(musicTime);
        query.addBindValue(musicUrl.toLocalFile());
        query.addBindValue(isLike ? 1 : 0);
        query.addBindValue(isHistory ? 1 : 0);
        if(!query.exec())
        {
            qDebug() << "更新失败" << query.lastError();
            return ;
        }
        qDebug() << "更新" << musicName;
    }

}

void Music::parseMediaMetaMusic()
{
    //创建媒体播放对象
    QMediaPlayer player;
    //依靠setMedia方法解析数据
    player.setMedia(musicUrl);
    //因为setMedia不会等待歌曲的加载就会立即返回  ，  因此此方法获取元数据时候需要保障数据加载完毕
    //需要通过 isMetaDataAvailable 确认一下
    while(!player.isMetaDataAvailable())
    {
        QCoreApplication::processEvents();
    }
    //while之后已经加载完成
    if(player.isMetaDataAvailable())
    {
        //将文件信息添加到Music成员里
        musicName = player.metaData("Title").toString();
        musicAlome = player.metaData("AlbumTitle").toString();
        musicSinger = player.metaData("Author").toString();
        musicTime = player.metaData("Duration").toLongLong();
        //在文件名称中找歌名    歌名 - 作者
        QString fileName = musicUrl.fileName();
        int index = fileName.indexOf("-");
        if(musicName.isEmpty())
        {
            if(index != -1)
            {
                musicName = fileName.mid(0,index);
            }
            else
            {
                musicName = fileName.mid(0,fileName.indexOf("."));
            }
        }
        //在文件名称中找作者    歌名 - 作者
        if(musicSinger.isEmpty())
        {
            if(index != -1)
            {
                musicSinger = fileName.mid(index+1,fileName.indexOf(".") - index - 1);
            }
            {
                musicSinger = "未知歌手";
            }
        }
        if(musicAlome.isEmpty())
        {
            musicAlome = "未知专辑";
        }

        //qDebug() << musicName << "+" << musicAlome << "+" <<  musicSinger << "+" <<  musicTime;
    }
}
