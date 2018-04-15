#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>//desktop使用
#include <QDebug>
#include "crc32/crc32.h"

#include "thread/qSendSerial.h"
#include "frameData/frameData.h"

MainWindow *w = NULL;

QSendSerial *sendSerial = NULL;
QMutex   linkMutex;




void testInit(void) {
    unsigned char s[100] = {0xf1, 0xf2, 0xf3, 0xf4, 0xf5};
    int len;

    len = 10;

    //qDebug("0X%08X", crc32(0, s, strlen(s)));
    //qDebug() << "test crc here";
    if (DealFrameData((char*)s, &len) == 0) {
        for (int i = 0; i < len; i++) {
            qDebug("transmit frameData[%d] is 0x%02x", i, 0xff & s[i]);
        }
    }
}


//大约需要10s来初始化设备
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    sendSerial = new QSendSerial();

    //sendSerial->run();

    w = new MainWindow();
    w->show();

    //w.move((QApplication::desktop()->width() - w.width())/2,(QApplication::desktop()->height() - w.height())/2);
    w->move((QApplication::desktop()->width() - w->width()*10/9),0);

    //线程启动
    sendSerial->start(QThread::LowPriority);

    testInit();
#define test_led
#if 0                                                                      //
    //char buf[40] = {0x2b, 0x69, 0x70, 0x64, 0x2c, 0x30, 0x2c, 0x31, 0x34, 0x3a, 0xfd, 20, 0x0, 0x1, 0x0, 0x2,      0x0d, 0x0a, 0x6f, 0x6b};
    //char buf[40] = "+IPD,0,14:send to server\r\nok";
#if defined(test_string_send)
    //send string to
    char buf[100] = "sEnD to server";
    int len = strlen(buf);
    //char buf[100] = {0x1, 0x22, 0x78, 0x0, 0x7};
    //int len = 5;
#elif defined(test_led)
    //open led                   led0 led1 led2  led3 led4
    char buf[100] = {0x0c, 0x01, 0x00, 0x1, 0x0, 0x1, 0x1};
    int len = 7;
#endif

    DealFrameData(buf, &len);


    char tmps[100] = "+IPD,0,24:";
    memcpy(&tmps[10], buf, len);
    memcpy(&tmps[len + 10], "\r\nOK", 4);
    qDebug() << QString(tmps);
    w->RawDealFrameData(QByteArray::fromRawData(tmps, len + 10 + 4));

#endif


    //w->slot_newDataReceived(QByteArray::fromRawData(tmps, len + 10 + 4));
    qDebug () << "slot";
    return a.exec();
}
