#include "musiclist.h"
#include<QMimeDatabase>
#include<QSqlQuery>
#include<QDebug>
MusicList::MusicList()
{

}

void MusicList::addMusicsByUrl(const QList<QUrl> url)
{
    for(auto e : url)
    {
        //如果存在就不再添加了
//        auto it = musicList.begin();
//        for(; it != musicList.end() ; it++)
//        {
//            if(e == it->getMusicQUrl())
//            {
//                return;
//            }
//        }
//        if(it != musicList.end())
//        {
//            continue;
//        }

        QString music_path = e.toLocalFile();
        //控件换时间
        if(musicPath.contains(music_path))
        {
            continue;
        }
        //如果是一个有效 歌曲文件再添加到Music List中
        //通过检测歌曲文件的MIME类型实现

        musicPath.insert(music_path);
        QMimeDatabase mimeDB;
        QMimeType mimeType = mimeDB.mimeTypeForFile(e.toLocalFile());
        QString mime = mimeType.name();
        if(mime == "audio/mpeg" || mime == "audio/flac" || mime == "audio/wav")
        {
            Music music(e);
            musicList.push_back(music);
        }

    }
}

QVector<Music>::iterator MusicList::begin()
{
    return musicList.begin();
}

QVector<Music>::iterator MusicList::end()
{
    return musicList.end();
}

QVector<Music>::iterator MusicList::findMusicById(const QString& MusicId)
{
    for(auto it = musicList.begin() ; it != musicList.end() ; it++)
    {
        if(it->getMusicUUID() == MusicId)
        {
            return it;
        }
    }
    return musicList.end();
}

void MusicList::writeMusicListToMpufferSql()
{
    for(auto e : musicList)
    {
        e.InsertMusicDB();
    }
}

void MusicList::readMusicListToMpufferSql()
{
    QSqlQuery query;
    query.prepare("SELECT musicId,musicName,musicSinger,musicAlome,musicTime,musicUrl,isLike,isHistory FROM MusicInfo");
    if(!query.exec())
    {
        qDebug() << "数据库查询失败";
        return ;
    }
    while(query.next())
    {

        Music music;
        music.setMusicId(query.value(0).toString());
        music.setMusicName(query.value(1).toString());
        music.setMusicSinger(query.value(2).toString());
        music.setMusicAlome(query.value(3).toString());
        music.setMusicTime(query.value(4).toLongLong());
        //music.setMusicQUrl(query.value(5).toString());
        music.setMusicLike(query.value(6).toBool());
        music.setMusicHistory(query.value(7).toBool());


        QString pathStr = query.value(5).toString();
        // 转成合法本地QUrl
        QUrl realUrl = QUrl::fromLocalFile(pathStr);
        // 给music赋值正确url
        music.setMusicQUrl(realUrl);
        // 安全取出本地绝对路径，放进musicPath
        musicPath.insert(realUrl.toLocalFile());
        musicList.push_back(music);

    }
}














