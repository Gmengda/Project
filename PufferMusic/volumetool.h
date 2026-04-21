#ifndef VOLUMETOOL_H
#define VOLUMETOOL_H

#include <QWidget>

namespace Ui {
class volumeTool;
}

class volumeTool : public QWidget
{
    Q_OBJECT

public:
    explicit volumeTool(QWidget *parent = nullptr);
    ~volumeTool();
    void paintEvent(QPaintEvent *event);
    void on_btnClick();
    bool eventFilter(QObject* watched,QEvent* event);

    //计算音量
    void calcVolume();

signals:
    void setMusicMuted(bool);
    void setMusicVolume(int volume);

private:
    Ui::volumeTool *ui;
    bool isMuted;
    int volue;
};

#endif // VOLUMETOOL_H
