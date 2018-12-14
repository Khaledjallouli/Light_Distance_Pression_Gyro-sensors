#include <QtWidgets/QApplication>
#include "mainwindowd.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowd w(0,800,600);
    MainWindow wTest;
    w.show();
    wTest.show();
    return a.exec();
}
