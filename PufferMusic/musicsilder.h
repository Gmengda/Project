#ifndef MUSICSILDER_H
#define MUSICSILDER_H

#include <QWidget>

namespace Ui {
class musicSilder;
}

class musicSilder : public QWidget
{
    Q_OBJECT

public:
    explicit musicSilder(QWidget *parent = nullptr);
    ~musicSilder();
    void silderScoendLoop(float ratio);

signals:
    void musicPosition(float ratio);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void onSilderChing();


private:
    Ui::musicSilder *ui;
    int position;
    int maxWidth;
};

#endif // MUSICSILDER_H
