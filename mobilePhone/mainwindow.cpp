#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "client/client.h"
#include "frameData/frameData.h"



int globalLongConnectFlag = 0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cli = new client();
    connect(cli, SIGNAL(emit_receive(QByteArray)), this, SLOT(ShowReceiveStr(QByteArray)));
    connect(cli, SIGNAL(now_sate(QByteArray)), this, SLOT(ShowReceiveStr(QByteArray)));

    ledStatus = 0;

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_openLed1_clicked()
{
    static unsigned char led1Status = 0;

    if (led1Status == 0) {

        led1Status = 1;
    } else {
        led1Status = 0;
    }

    ledStatus &= ~(1<<0);
    ledStatus |= led1Status << 0;

    CommonSendLed();
}

void MainWindow::CommonSendLed()
{


    QString ip = ui->lineEdit_ip->text();

    int port = ui->lineEdit_port->text().toInt();



    char buf[256] = {0};
    int len = 0;

    buf[0] = 0x0c;
    buf[1] = 0x01;
    for (int i = 0; i < 8; i++) {
        buf[i+2] = (ledStatus>>i) & 0x01;
    }
    len = 10;

    if (DealFrameData(buf, &len) == 0) {
        recordTmp = QByteArray::fromRawData(buf, len);
//        for (int i = 0; i < tmp.size(); i++) {
//            qDebug("data[%d] is %02x", i, tmp.at(i));

//        }
        cli->transmit_info(ip, port, recordTmp);
    } else {
        QByteArray tmp = "error here";
        cli->transmit_info(ip, port, tmp);
    }
    qDebug() << "Open led1";

}


void MainWindow::on_pushButton_openLed2_clicked()
{
    static unsigned char led2Status = 0;

    if (led2Status == 0) {

        led2Status = 1;
    } else {
        led2Status = 0;
    }

    ledStatus &= ~(1<<1);
    ledStatus |= led2Status << 1;

    CommonSendLed();
    qDebug() << "Open led2";
}

void MainWindow::on_pushButton_openLed3_clicked()
{
    static unsigned char led3Status = 0;

    if (led3Status == 0) {

        led3Status = 1;
    } else {
        led3Status = 0;
    }

    ledStatus &= ~(1<<2);
    ledStatus |= led3Status << 2;

    CommonSendLed();
    qDebug() << "Open led3";
}

void MainWindow::on_pushButton_openLed4_clicked()
{
    static unsigned char led4Status = 0;

    if (led4Status == 0) {

        led4Status = 1;
    } else {
        led4Status = 0;
    }

    ledStatus &= ~(1<<3);
    ledStatus |= led4Status << 3;

    CommonSendLed();
    qDebug() << "Open led4";
}

void MainWindow::on_pushButton_openLed5_clicked()
{
    static unsigned char led5Status = 0;

    if (led5Status == 0) {

        led5Status = 1;
    } else {
        led5Status = 0;
    }

    ledStatus &= ~(1<<4);
    ledStatus |= led5Status << 4;

    CommonSendLed();
    qDebug() << "Open led5";
}


void MainWindow::on_pushButton_sendStr_clicked()
{
    QString ip = ui->lineEdit_ip->text();

    int port = ui->lineEdit_port->text().toInt();

    //int timeout = 10;
    //int tryCount = 5;

    QString send = ui->lineEdit_sendStr->text();


    if (ui->checkBox_longConnect->checkState() == Qt::Checked) {
        globalLongConnectFlag = 2;
    } else {
        globalLongConnectFlag = 1;

    }

    //发送帧数据
    if (ui->checkBox_frameData->checkState() == Qt::Checked) {
        char buf[1000] = {0};
        int len = 0;
        strcpy(buf, send.toLatin1());
        len = strlen(buf);
        if (DealFrameData(buf, &len) == 0) {
            QByteArray tmp = QByteArray::fromRawData(buf, len);
            for (int i = 0; i < tmp.size(); i++) {
                qDebug("data[%d] is %02x", i, tmp.at(i));

            }
            cli->transmit_info(ip, port, tmp);
        } else {
            QByteArray tmp = send.toLatin1();
            cli->transmit_info(ip, port, tmp);
        }
    } else {
        QByteArray tmp = send.toLatin1();
        cli->transmit_info(ip, port, tmp);

    }
}

void MainWindow::on_pushButton_connect_clicked()
{
    QString ip = ui->lineEdit_ip->text();

    int port = ui->lineEdit_port->text().toInt();

    int timeout = 10;
    int tryCount = 5;

    globalLongConnectFlag = 1;

    cli->test_server_port(ip, port, timeout, tryCount);

    qDebug() << "test server connect";
}


void MainWindow::ShowReceiveStr(QByteArray rec) {
    unsigned char buf[1024] = {0};

    ui->textEdit_showInfo->insertPlainText(rec);

    if (rec.indexOf("test connect ok here") >= 0) {
        ui->label_connectStatus->setStyleSheet("background-color: green");

    } else if (rec.indexOf("time out here") >= 0) {
        ui->label_connectStatus->setStyleSheet("background-color: red");
    }

    memcpy(buf, rec.data(), rec.size());
    if ((buf[0] == TRANSPARENT_CMD)) {
        qDebug() << "transparent send here";

    }
    for (int i = 0; i < rec.size(); i++) {
        qDebug("buf[%d] = 0x%x, %c", i, 0xff&buf[i], buf[i]);

    }

    if (rec.indexOf("ok") >= 0) {
        qDebug("get ok here %x", ledStatus);
        if (((ledStatus >> 0) & 0x1) == 1) {
            ui->label_led1->setStyleSheet("background-color: green");

        } else if (((ledStatus >> 0) & 0x1) == 0) {
            ui->label_led1->setStyleSheet("background-color: red");
        }

        if (((ledStatus >> 1) & 0x1) == 1) {
            ui->label_led2->setStyleSheet("background-color: green");

        } else if (((ledStatus >> 1) & 0x1) == 0) {
            ui->label_led2->setStyleSheet("background-color: red");
        }

        if (((ledStatus >> 2) & 0x1) == 1) {
            ui->label_led3->setStyleSheet("background-color: green");

        } else if (((ledStatus >> 2) & 0x1) == 0) {
            ui->label_led3->setStyleSheet("background-color: red");
        }

        if (((ledStatus >> 3) & 0x1) == 1) {
            ui->label_led4->setStyleSheet("background-color: green");

        } else if (((ledStatus >> 3) & 0x1) == 0) {
            ui->label_led4->setStyleSheet("background-color: red");
        }


        if (((ledStatus >> 4) & 0x1) == 1) {
            ui->label_led5->setStyleSheet("background-color: green");

        } else if (((ledStatus >> 4) & 0x1) == 0) {
            ui->label_led5->setStyleSheet("background-color: red");
        }
    } else {
        qDebug() << "not get ok here";
    }

}



void MainWindow::on_pushButton_clearZone_clicked()
{
    ui->textEdit_showInfo->setText("");
}

void MainWindow::on_pushButton_closeTcp_clicked()
{
    cli->RawCloseTcp();
}
