#include "piguider.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PiGuider w;
    w.show();

    return a.exec();
}
