#ifndef LISTITEMBOX_H
#define LISTITEMBOX_H

#include <QWidget>

namespace Ui {
class listItemBox;
}

class listItemBox : public QWidget
{
    Q_OBJECT

public:
    explicit listItemBox(QWidget *parent = nullptr);
    ~listItemBox();
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void setMusicName(const QString& musicname);
    void setMusicSinger(const QString& musicsinger);
    void setMusicAlome(const QString& musicalome);
    void setIsLike(bool islike);

    void onBtnClick();

signals:
    void setMusicIsLike(bool ilike);

private:
    Ui::listItemBox *ui;
    bool isLike;
};

#endif // LISTITEMBOX_H
