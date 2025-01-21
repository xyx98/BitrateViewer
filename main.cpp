#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
#ifdef FORCE_ENG_VIR
    qputenv("LANG", "en_US.UTF8");
    qputenv("LANGUAGE", "en_US");
#endif
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
