#include "lrcpage.h"
#include "ui_lrcpage.h"
#include <QFile>
#include <QDebug>
lrcPage::lrcPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::lrcPage)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
//    lrcPageAnimal = new QPropertyAnimation(this,"geometry",this);
//    lrcPageAnimal->setDuration(250);
//    lrcPageAnimal->setStartValue(QRect(0,0,this->width(),this->height()));
//    lrcPageAnimal->setEndValue(QRect(0,0 + this->height(),this->width(),this->height()));
    connect(ui->exit,&QPushButton::clicked,this,[=](){
//        lrcPageAnimal->start();
        this->hide();
    });
//    connect(lrcPageAnimal,&QPropertyAnimation::finished,this,[=](){
//        this->hide();
//    });

    showLrcWord(-1);
}

lrcPage::~lrcPage()
{
    delete ui;
}

void lrcPage::parseLrcFile(const QString& lrcUrl)
{
    lrcLineWords.clear();
    //打开文件
    QFile file(lrcUrl);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "打开lrc文件失败";
        return ;
    }
    while(!file.atEnd())
    {
        //提取一行歌词
        QString lrcLineWord = file.readLine(1024);
        //解析歌词
        //1分开时间和文本
        //2在时间中吧分秒毫秒拆开
        //3保存
        //1
        int start = 0;
        int end = 0;
        end = lrcLineWord.indexOf(QChar(']'),start);
        QString Timepos = lrcLineWord.mid(start,end - start + 1);
        QString Wordpos = lrcLineWord.mid(end + 1,lrcLineWord.size() - end - 1 - 1 - 1);
        //2
        start = 1;
        end = lrcLineWord.indexOf(QChar(':'),start);
        int ms = lrcLineWord.mid(start,end - start).toUInt()*60*1000;
        start = end+1;
        end = lrcLineWord.indexOf(QChar('.'),start);
        ms += lrcLineWord.mid(start,end - start).toUInt()*1000;
        start = end+1;
        end = lrcLineWord.indexOf(QChar('.'),start);
        ms += lrcLineWord.mid(start,end - start).toUInt();
        //3
        lrcLineWords.push_back(LLrcLineWord(ms,Wordpos));
    }
    for(auto e : lrcLineWords)
    {
        qDebug() << e.lrctext << "+" << e.lrctime;
    }
}

void lrcPage::showLrcWord(qint64 position)
{
    int index = getlrcIndex(position);
    if(index == -1)
    {
        ui->line1->setText(QString(""));
        ui->line2->setText(QString(""));
        ui->line3->setText(QString(""));
        ui->line4->setText(QString("暂无歌词"));
        ui->line5->setText(QString(""));
        ui->line6->setText(QString(""));
        ui->line7->setText(QString(""));
    }
    else
    {
        ui->line1->setText(getLrcWordOfIndex(index-3));
        ui->line2->setText(getLrcWordOfIndex(index-2));
        ui->line3->setText(getLrcWordOfIndex(index-1));
        ui->line4->setText(getLrcWordOfIndex(index));
        ui->line5->setText(getLrcWordOfIndex(index+1));
        ui->line6->setText(getLrcWordOfIndex(index+2));
        ui->line7->setText(getLrcWordOfIndex(index+3));
    }
}

int lrcPage::getlrcIndex(qint64 position)
{
    if(lrcLineWords.isEmpty())
    {
        return -1;
    }
    int i = 0;
    for(auto e : lrcLineWords)
    {
        if(position <= e.lrctime)
        {
            return i;
        }
        i++;
    }
    return lrcLineWords.size() - 1;
}

QString lrcPage::getLrcWordOfIndex(qint64 index)
{
    if(index <= 0 || index > lrcLineWords.size())
    {
        return "";
    }
    return lrcLineWords[index].lrctext;
}














