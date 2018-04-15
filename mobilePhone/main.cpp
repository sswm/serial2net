#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include "client/client.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int currentScreenWid = QApplication::desktop()->width();
    int currentScreenHei  = QApplication::desktop()->height();
    qDebug() << "currentScreenWid is " << currentScreenWid;
    qDebug() << "currentScreenHei is " << currentScreenHei;


    MainWindow w;
    w.show();

    return a.exec();
}
