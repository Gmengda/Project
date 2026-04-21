#ifndef RECBOXITEM_H
#define RECBOXITEM_H
#include <QPropertyAnimation>
#include <QWidget>

namespace Ui {
class RecBoxItem;
}

class RecBoxItem : public QWidget
{
    Q_OBJECT

public:
    explicit RecBoxItem(QWidget *parent = nullptr);
    ~RecBoxItem();
    bool eventFilter(QObject *watched, QEvent *event);
    void setRecText(const QString& text);
    void setRecImage(const QString& imageurl);

private:
    Ui::RecBoxItem *ui;
};

#endif // RECBOXITEM_H
