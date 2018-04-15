#include "qSendSerial.h"

#include "mainwindow.h"
#include <QDebug>

#include <windows.h>// sleep

#include <QDateTime>

extern MainWindow *w;

extern unsigned char needToEmitFlag;

/*
流程

A线程一次存入多条数据
B线程依次取出一条数据，发送给A线程来发送
A线程完成接受后，告诉B线程可以取下一条数据了
*/
QSendSerial::QSendSerial()
{
    ThreadInit();
}


QSendSerial::~QSendSerial()
{



}



void QSendSerial::ThreadInit() {

        qDebug() << "Thread init here";

}



void QSendSerial::SendTimeoutSignal() {

    qDebug() << "send signal here";
    //receiveTimeoutTimer->stop();//在此线程中停止,因为有可能是其他线程调用此函数
    //QTimer::singleShot(0, receiveTimeoutTimer,SLOT(stop()));
    quit();
}

void QSendSerial::run()
{
    char bufTmp[ONE_FRAME_SIZE];
    int timeout;
    int rawSize;

    ::Sleep(2000);

    receiveTimeoutTimer = new QTimer();

    connect(receiveTimeoutTimer, SIGNAL(timeout()), this, SLOT(SendTimeoutSignal()));


    receiveTimeoutTimer->setInterval(2*1000);


    //exec();
    qDebug() << "QSendSerial thread start here";
    if (w == NULL) {
        qDebug() << "error";

    }
    //w->on_pushButton_listAp_clicked();
    //w->refreshCom();



    connect(this, SIGNAL(TryToSendData(QByteArray)), w, SLOT(RawSendData(QByteArray)));
    //错误格式
    //connect(this, SIGNAL(TryToSendData(QByteArray tmp)), w, SLOT(RawSendData(QByteArray tmp)));



    while(1) {
        //

        if (w->GetFromMsgStart(bufTmp, &timeout, &rawSize) != -1) {
            qDebug("Command:%s:end", bufTmp);
            //w->RawSendData(bufTmp);
            needToEmitFlag = 1;//为1的情况下,才能发送emit
            emit  TryToSendData(QByteArray::fromRawData(bufTmp, rawSize));//让主线程来发送数据
            //receiveTimeoutTimer->stop();
            //QTimer::singleShot(0, receiveTimeoutTimer,SLOT(stop()));
            //timeout *= 1000;
            //QTimer::singleShot(0, receiveTimeoutTimer,SLOT(setInterval(timeout)));
            //receiveTimeoutTimer->setInterval(timeout*1000);
            //QTimer::singleShot(0);
            //receiveTimeoutTimer->start(2*1000);
            //触发另一线程的定时器
            //qDebug() <<timeout;
            receiveTimeoutTimer->start(timeout*1000);
            //QTimer::singleShot(0, receiveTimeoutTimer,SLOT(start()));
            qDebug() << "wait here";
            exec();
            needToEmitFlag = 0;
            receiveTimeoutTimer->stop();

            QDateTime time = QDateTime::currentDateTime();
            qDebug() << "~~~~~~~~~receive quit here~~~" + time.toLocalTime().toString("yyyy-MM-dd hh:mm:ss ddd");

            qDebug() << "quit";
            //发送数据
            /**/


            //停留在此等待信号
        }
        //::Sleep(1000);
    }




}
