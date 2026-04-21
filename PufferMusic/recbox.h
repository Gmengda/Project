#ifndef RECBOX_H
#define RECBOX_H
#include <QPropertyAnimation>
#include <QWidget>
#include <QJsonArray>

namespace Ui {
class RecBox;
}

class RecBox : public QWidget
{
    Q_OBJECT

public:
    explicit RecBox(QWidget *parent = nullptr);
    ~RecBox();
    void initRecBoxUi(QJsonArray data,int row);

private slots:
    void on_btnDown_clicked();

    void on_btnUp_clicked();

private:
    void creatRecBoxItem();

private:
    Ui::RecBox *ui;
    int row;
    int col;
    QJsonArray imageList;
    int currentIndex; //第几组图片
    int count;        //一共几组
};

#endif // RECBOX_H
