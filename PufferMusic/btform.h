#ifndef BTFORM_H
#define BTFORM_H
#include <QPropertyAnimation>
#include <QWidget>

namespace Ui {
class BtForm;
}

class BtForm : public QWidget
{
    Q_OBJECT
signals:
    void btClick(int id);

public:
    explicit BtForm(QWidget *parent = nullptr);
    ~BtForm();
    void setButIconAndText(const QString& QIcon,const QString& QText,int pageIdClick);
    int getPageId()const;
    void clearBackground();
    void showAnmial(bool isShow);
protected:
    void mousePressEvent(QMouseEvent *event);
private:
    Ui::BtForm *ui;
    int pageId;
    QPropertyAnimation* animal1;
    QPropertyAnimation* animal2;
    QPropertyAnimation* animal3;
    QPropertyAnimation* animal4;
};

#endif // BTFORM_H
