#include <QtGui/QApplication>
#include "QtMainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtMainwindow w;
    w.show();

    return a.exec();
}

