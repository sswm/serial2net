#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>//desktop使用
#include <QDebug>
#include <QScrollArea>

#include "crc32/crc32.h"

#include "thread/qSendSerial.h"

MainWindow *w = NULL;

QSendSerial *sendSerial = NULL;
QMutex   linkMutex;




void testInit(void) {
    char s[] = "12345";

     qDebug("0X%08X", crc32(0, s, strlen(s)));
     qDebug() << "test crc here";

}


//大约需要10s来初始化设备
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    sendSerial = new QSendSerial();

    //sendSerial->run();

    w = new MainWindow();
    //w->show();

    //w.move((QApplication::desktop()->width() - w.width())/2,(QApplication::desktop()->height() - w.height())/2);
    //w->move((QApplication::desktop()->width() - w->width()*4/3),0);

    QTabWidget *tabwidget = new QTabWidget;
    QScrollArea *scrollArea = new QScrollArea;
    //scrollArea->resize(900,1050);
    scrollArea->setWidget(w);
    //scrollArea->show();

    //页的方式显示
    tabwidget->addTab(scrollArea,"串口wifi");
    tabwidget->resize(950,700);
    tabwidget->move((QApplication::desktop()->width() - w->width()*4/3),0);
    tabwidget->show();


    //线程启动
    sendSerial->start(QThread::LowPriority);


    testInit();

    return a.exec();
}
