#include "client.h"

static int delete_count = 0;

extern int globalLongConnectFlag;


client::client()
{
    // 1
    this->tcp_socket = new QTcpSocket(this);
}


void client::transmit_info(QString serverip, int port, QByteArray stringSend)
{
    connect_timeout.stop();
    this->serverip = serverip;
    this->port = port;
    this->cmd = stringSend;
    this->time_out = TEST_TIME_OUT;
    error_timeout = false;


    error_flag = false;
    init();
    //error_timeout = false;

    bufTmpSize = cmd.size();
    for (int i = 0; i < cmd.size(); i++) {
        qDebug("cmd[%d] is %02x", i, cmd.at(i));
        bufTmp[i] = cmd.at(i);
    }

}




void client::test_server_port(QString serverip, int port, int sig_sec, int num)
{
    qDebug() << "testing the" << serverip << "port:" << port;
    this->connect_state     = false;
    this->serverip          = serverip;
    this->port              = port;
    this->time_out          = sig_sec;
    this->time_out_count    = num;
    this->now_time_out_count= 0;
    this->error_flag        = false;
    // 1

    //connect_timeout.stop();
    connect(&connect_timeout, SIGNAL(timeout()), this, SLOT(test_connect_to_host_timeout()));
    // 2
    //this->tcp_socket = new QTcpSocket(this);
    try_connect();
}

void client::try_connect()
{


    connect(tcp_socket, SIGNAL(connected()), this, SLOT(emit_connect_state()));
    //connect(tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(test_display_error(QAbstractSocket::SocketError)));
    tcp_socket->connectToHost(this->serverip, this->port);
    connect_timeout.start(time_out*1000);
    //connect(tcp_socket, SIGNAL(connected()), this, SLOT(emit_connect_state()));
    //connect(tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(test_display_error(QAbstractSocket::SocketError)));

}


void client::test_display_error(QAbstractSocket::SocketError)
{
    tcp_socket->close();
    qDebug() << "test_display error occur";
    qDebug() << tcp_socket->errorString();
}

client::~client()
{

}

void client::RawCloseTcp() {
    connect_timeout.stop();
    disconnect(&connect_timeout, SIGNAL(timeout()), this, SLOT(connect_to_host_timeout()));
    disconnect(tcp_socket, SIGNAL(connected()), this, SLOT(cmd_write()));
    disconnect(tcp_socket, SIGNAL(readyRead()), this, SLOT(rec_data()));
    if (globalLongConnectFlag == 2) {
        disconnect(tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(display_error(QAbstractSocket::SocketError)));
    }

    qDebug() << tcp_socket->state();
    if (tcp_socket->state() == QAbstractSocket::ConnectedState) {
        tcp_socket->close();;
    } else {
        ;

    }

    emit emit_receive("tcp close");


}
void client::emit_connect_state()
{
    connect_timeout.stop();
    disconnect(tcp_socket, SIGNAL(connected()), this, SLOT(emit_connect_state()));
    disconnect(&connect_timeout, SIGNAL(timeout()), this, SLOT(test_connect_to_host_timeout()));
    //disconnect(tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(test_display_error(QAbstractSocket::SocketError)));
    this->connect_state = true;
    tcp_socket->close();


    if (tcp_socket != NULL) {
        delete_count++;
        //delete tcp_socket;
        // tcp_socket = NULL;
        //qDebug("delete the tcp_socket %d times", delete_count);
    }
    QString tmp = "test connect ok here";
    qDebug() << tmp;
    emit now_sate(tmp.toLatin1());

}



void client::new_tcp_connection()
{
    //qDebug() << "here?";
    //qDebug() << serverip;
    //qDebug() << tcp_socket;
    //tcp_socket->abort();
    //qDebug("time out is %d", time_out);
    tcp_socket->connectToHost(this->serverip, this->port);

    if (globalLongConnectFlag == 2) {

        qDebug() << "not start timer here";
    } else  {
        connect_timeout.start(time_out*1000);
    }
    //qDebug() << "start the time_out";
    //tcp_socket->connectToHost("192.168.1.59",HTTP_PORT);

}


void client::test_connect_to_host_timeout()
{
    this->connect_state = false;

    connect_timeout.stop();
    tcp_socket->abort();
    disconnect(tcp_socket, SIGNAL(connected()), this, SLOT(emit_connect_state()));

    //disconnect(tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(test_display_error(QAbstractSocket::SocketError)));



    error_flag = true;
    tcp_socket->close();
    qDebug() << "please check the host ";
    this->now_time_out_count++;
    qDebug() << "test connect to host timeout ";
    qDebug("fail %d time", now_time_out_count);

    if (now_time_out_count >= time_out_count) {
        disconnect(&connect_timeout, SIGNAL(timeout()), this, SLOT(test_connect_to_host_timeout()));
        qDebug("\n=====================================================\n");
        qDebug("connect to host for %d second and connect fail", time_out*time_out_count);
        qDebug("\n=====================================================\n");

        if (tcp_socket != NULL) {
            delete_count++;
            //delete tcp_socket;
            //tcp_socket = NULL;
            qDebug("delete the test tcp_socket %d times", delete_count);
        }

        emit now_sate("time out here");
    } else {
        try_connect();
    }
}

void client::connect_to_host_timeout()
{
    QString tmp;

    connect_timeout.stop();
    disconnect(&connect_timeout, SIGNAL(timeout()), this, SLOT(connect_to_host_timeout()));
    disconnect(tcp_socket, SIGNAL(connected()), this, SLOT(cmd_write()));
    disconnect(tcp_socket, SIGNAL(readyRead()), this, SLOT(rec_data()));


    tmp = "connect to host timeout";
    qDebug() << tmp;
    qDebug() << "please check the host ";
    //connect(tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(display_error(QAbstractSocket::SocketError)));
    //emit tcp_socket->error(QAbstractSocket::SocketError);

    error_flag = true;
    error_timeout = true;

    tcp_socket->close();
    tcp_socket->abort();
    if (tcp_socket != NULL) {
        delete_count++;
        //delete tcp_socket;
        //tcp_socket = NULL;
        //qDebug("delete the tcp_socket %d times", delete_count);
    }

    emit emit_receive(tmp.toLatin1());


}



void client::init()
{

    // 1
    //this->tcp_socket = new QTcpSocket(this);
    // datas = "";
    // 2
    new_tcp_connection();
    connect(&connect_timeout, SIGNAL(timeout()), this, SLOT(connect_to_host_timeout()));
    //to avoid continue write error;
    connect(tcp_socket, SIGNAL(connected()), this, SLOT(cmd_write()));
    connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(rec_data()));

    if (globalLongConnectFlag == 2) {
        connect(tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(display_error(QAbstractSocket::SocketError)));
    }
    // ui->textEdit->setText("POST /cgi-bin/mclogin1.cgi HTTP/1.1\r\nContent-Length: 7\r\n\r\nt=a&b=c\r\n");


    //ui->textEdit->setText("POST /cgi-bin/mclogin1.cgi HTTP/1.1\r\nContent-Length: 7\r\n\r\nt=a&b=c\r\n");

    //will cause error if not connect
    //this->tcp_socket->write(cmd.toLatin1());


}

void client::cmd_write()
{
    //qDebug() << "write ?";

    for (int i = 0; i < cmd.size(); i++) {
        //为什么会不等？
        qDebug("before cmd[%d] is %02x %02x", i, bufTmp[i], cmd.at(i));

    }
    cmd = QByteArray::fromRawData(bufTmp, bufTmpSize);
    this->tcp_socket->write(this->cmd);
}


void client::display_error(QAbstractSocket::SocketError)
{
    QString tmp;
    qDebug() << "you are in error";
    qDebug() << "set the flag to true";

    disconnect(&connect_timeout, SIGNAL(timeout()), this, SLOT(connect_to_host_timeout()));
    //to avoid continue write error;
    disconnect(tcp_socket, SIGNAL(connected()), this, SLOT(cmd_write()));
    disconnect(tcp_socket, SIGNAL(readyRead()), this, SLOT(rec_data()));


    error_flag = true;
    qDebug() << tcp_socket->errorString();
    tmp = tcp_socket->errorString();
    if (tcp_socket->errorString().compare("Host unreachable") == 0) {
        qDebug() << "string is Host unreachable";
        tmp = tcp_socket->errorString();
    }
    tcp_socket->close();


    connect_timeout.stop();
    emit emit_receive(tmp.toLatin1());
    //delete this;
}

void client::rec_data()
{

    connect_timeout.stop();



    if (globalLongConnectFlag == 2) {
        ;
//        disconnect(tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(display_error(QAbstractSocket::SocketError)));
//        disconnect(&connect_timeout, SIGNAL(timeout()), this, SLOT(connect_to_host_timeout()));
//        disconnect(tcp_socket, SIGNAL(connected()), this, SLOT(cmd_write()));
//        disconnect(tcp_socket, SIGNAL(readyRead()), this, SLOT(rec_data()));
    } else {
        disconnect(&connect_timeout, SIGNAL(timeout()), this, SLOT(connect_to_host_timeout()));
        disconnect(tcp_socket, SIGNAL(connected()), this, SLOT(cmd_write()));
        disconnect(tcp_socket, SIGNAL(readyRead()), this, SLOT(rec_data()));
    }


    QByteArray da= tcp_socket->readAll();

    if (QString(da).compare("") == 0) {
        qDebug() << "receive is null error happen";
        da = "receive is null error happen";
    }
    //da = da + "\r\n\r\n=======================\r\n";
    //datas = datas + da;

    //qDebug() << da;

    if (globalLongConnectFlag != 2) {
        tcp_socket->close();
    }
    // if (tcp_socket != NULL) {
    //  delete_count++;
    //delete tcp_socket;
    //tcp_socket = NULL;
    //qDebug("delete the tcp_socket %d times", delete_count);
    //  }

    emit emit_receive(da);
    //ui->textEdit_2->setText(datas);
    //connect_timeout.start(3000);
}



