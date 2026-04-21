#include "puffermusic.h"

#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSharedMemory memoty("Puffer");

    if(memoty.attach())
    {
        QMessageBox::information(nullptr,"PufferMusic提示","软件已打开");
        return 0;
    }
    memoty.create(1);

    PufferMusic w;
    w.show();
    return a.exec();
}
