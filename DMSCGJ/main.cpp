#include "dmscgj.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DMSCGJ w;
    w.show();

    return a.exec();
}
