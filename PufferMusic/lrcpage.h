#ifndef LRCPAGE_H
#define LRCPAGE_H

#include <QWidget>
#include <QPropertyAnimation>

struct LLrcLineWord
{
    LLrcLineWord(qint64 time,QString text)
    {
        this->lrctime = time;
        this->lrctext = text;
    }
    qint64 lrctime;
    QString lrctext;
};

namespace Ui {
class lrcPage;
}

class lrcPage : public QWidget
{
    Q_OBJECT

public:
    explicit lrcPage(QWidget *parent = nullptr);
    ~lrcPage();
    void parseLrcFile(const QString& lrcUrl);
    void showLrcWord(qint64 position);
    int getlrcIndex(qint64 position);
    QString getLrcWordOfIndex(qint64 index);

private:
    Ui::lrcPage *ui;
    QPropertyAnimation* lrcPageAnimal;
    QVector<LLrcLineWord> lrcLineWords;
};

#endif // LRCPAGE_H
