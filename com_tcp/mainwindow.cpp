#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>
#include <QDebug>
#include <QTextCodec>
#include <QKeyEvent>
#include "crc32/crc32.h"
#include "md5/md5.h"
#include <mainwindow.h>
#include <QDateTime>
#include <QScrollArea>

#include "thread/qSendSerial.h"
//发送头初始化
postLinked   postMsgHead;

extern QSendSerial *sendSerial;


unsigned char needToEmitFlag = 0;

int MainWindow::AddToMsgEnd(char *str, unsigned int timeout) {

    linkMutex.lock();

    QDateTime time = QDateTime::currentDateTime();
    qDebug() << "~~~~~~~~~add msg here~~~" + time.toLocalTime().toString("yyyy-MM-dd hh:mm:ss ddd");

    postLinked *p;

    p = (postLinked *)malloc(sizeof(postLinked));

    if (p == NULL) {
        linkMutex.unlock();
        return -1;

    }

    strcpy(p->buf, str);
    p->timeout = timeout;

    list_add_tail (&p->list, &postMsgHead.list);



    linkMutex.unlock();

    return 0;
}

int MainWindow::GetFromMsgStart(char *str, int *timeout) {

    linkMutex.lock();


    postLinked *p;

    struct list_head *ptr;

    ptr = postMsgHead.list.next;



    //返回type类型中有链表所在结构体的结构体地址
    p = list_entry(ptr, postLinked, list);

    if (p == &postMsgHead) {
        linkMutex.unlock();
        return -1;

    }


    strcpy(str, p->buf);
    *timeout = p->timeout;

    list_del(ptr);

    //释放空间
    free(p);


    linkMutex.unlock();
    return 0;
}


void MainWindow::ShowSystemTime(void) {

    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间

    QString str = time.toString("yyyy-MM-dd hh:mm:ss ddd"); //设置显示格式
    ui->label_time->setText(str);//在标签上显示时间

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

//    QTabWidget *tabwidget = new QTabWidget;
//    QScrollArea *scrollArea = new QScrollArea;
//    //this->resize(900,1050);
//    scrollArea->setWidget(this);
//    scrollArea->show();

//    //页的方式显示
//    tabwidget->addTab(scrollArea,"串口wifi");
//    tabwidget->show();

    //初始化链表
    INIT_LIST_HEAD(&postMsgHead.list);


    //    AddToMsgEnd("first add here", 3);
    //    AddToMsgEnd("second add here", 4);
    //    AddToMsgEnd("third add here", 5);
    connect(this, SIGNAL(ReceiveMisString()), sendSerial, SLOT(SendTimeoutSignal()));

    //connect(sendSerial, SIGNAL(TryToSendData(QByteArray tmp)), this, SLOT(//RawSendData(QByteArray tmp)));


    //透传模式//不适用透传
    transparentMode = false;


    firstInit();

    setWindowIcon(QIcon(":/images/com_tcp.ico"));

    openStatus = false;
    showFlag = true;
    ui->label_stopShow->setStyleSheet("background-color: green");


    autoTimer = new QTimer();

    delayTimer = new QTimer();

    autoOpenTimer = new QTimer();

    delayDealStringTimer = new QTimer();

    showTimer = new QTimer();

    connect(showTimer, SIGNAL(timeout()), this, SLOT(ShowSystemTime()));
    showTimer->setInterval(1000);
    showTimer->start();

    //自动发送
    connect(autoTimer, SIGNAL(timeout()), this, SLOT(on_pushButton_manualSend_clicked()));
    //延时设置定时时常
    connect(delayTimer, SIGNAL(timeout()), this, SLOT(DelaySetTimer()));


    connect(autoOpenTimer, SIGNAL(timeout()), this, SLOT(on_pushButton_openClose_clicked()));
    //点击检测端口时，检测到有端口的话，1s后自动打开最后一个端口   会导致先关闭当前串口，再打开最后检测到的串口
    autoOpenTimer->setInterval(1000);


    connect(delayDealStringTimer, SIGNAL(timeout()), this, SLOT(DealRecDepCmd()));
    delayDealStringTime = DEFAULT_DEALY;
    delayDealStringTimer->setInterval(delayDealStringTime);


    statisNum = 0;

    comboBoxPortName = ui->comboBox_com;//prot
    comboBoxBaudRate = ui->comboBox_rate;//baudrate
    comboBoxParity = ui->comboBox_crc;//crc
    comboBoxDataBits = ui->comboBox_dataSize;//
    comboBoxStopBits = ui->comboBox_stopBit;
    comboBoxFlowControl = ui->comboBox_flow;//flow


    textEditSend = ui->textEdit_sendZone;
    textEditReceive = ui->textEdit_recZone;

    checkBoxCR = ui->checkBox_CR;
    checkBoxLF = ui->checkBox_LF;


    comStatus = ui->label_comStatus;

    serialPort = new ManageSerialPort;
    connect(serialPort, SIGNAL(newDataReceived(const QByteArray &)), this, SLOT(slot_newDataReceived(const QByteArray &)));





    //comboBoxPortName->removeItem();
    comboBoxPortName->clear();
    //window=======================
    comboBoxPortName->addItem("COM1", QVariant::fromValue(QString("com1")));
    comboBoxPortName->addItem("COM2", QVariant::fromValue(QString("com2")));
    comboBoxPortName->addItem("COM3", QVariant::fromValue(QString("com3")));
    comboBoxPortName->addItem("COM4", QVariant::fromValue(QString("com4")));
    comboBoxPortName->addItem("COM5", QVariant::fromValue(QString("com5")));
    comboBoxPortName->addItem("COM6", QVariant::fromValue(QString("com6")));
    comboBoxPortName->addItem("COM7", QVariant::fromValue(QString("com7")));
    comboBoxPortName->addItem("COM8", QVariant::fromValue(QString("com8")));
    comboBoxPortName->addItem("COM9", QVariant::fromValue(QString("com9")));
    comboBoxPortName->addItem("COM10", QVariant::fromValue(QString("com10")));
    //linux=========================
    //comboBoxPortName->addItem("/dev/ttyS0", QVariant::fromValue(QString("/dev/ttyS0")));
    //comboBoxPortName->addItem("/dev/ttyS1", QVariant::fromValue(QString("/dev/ttyS1")));
    //    comboBoxPortName->addItem("ttyS0", QVariant::fromValue(QString("/dev/s3c2410_serial0")));
    //    comboBoxPortName->addItem("ttyS1", QVariant::fromValue(QString("/dev/s3c2410_serial1")));
    //    comboBoxPortName->addItem("ttyS2", QVariant::fromValue(QString("/dev/s3c2410_serial2")));


    //refreshCom();
    on_pushButton_checkCom_clicked();



    comboBoxBaudRate->clear();
    comboBoxBaudRate->addItem("110", QVariant::fromValue(BAUD110));
    comboBoxBaudRate->addItem("300", QVariant::fromValue(BAUD300));
    comboBoxBaudRate->addItem("600", QVariant::fromValue(BAUD600));
    comboBoxBaudRate->addItem("1200", QVariant::fromValue(BAUD1200));
    comboBoxBaudRate->addItem("2400", QVariant::fromValue(BAUD2400));
    comboBoxBaudRate->addItem("4800", QVariant::fromValue(BAUD4800));
    comboBoxBaudRate->addItem("9600", QVariant::fromValue(BAUD9600));
    comboBoxBaudRate->addItem("19200", QVariant::fromValue(BAUD19200));
    comboBoxBaudRate->addItem("38400", QVariant::fromValue(BAUD38400));
    comboBoxBaudRate->addItem("57600", QVariant::fromValue(BAUD57600));
    comboBoxBaudRate->addItem("115200", QVariant::fromValue(BAUD115200));
    comboBoxBaudRate->setCurrentIndex(6); //9600 bauds

    comboBoxDataBits->clear();
    comboBoxDataBits->addItem("5", QVariant::fromValue(DATA_5));
    comboBoxDataBits->addItem("6", QVariant::fromValue(DATA_6));
    comboBoxDataBits->addItem("7", QVariant::fromValue(DATA_7));
    comboBoxDataBits->addItem("8", QVariant::fromValue(DATA_8));
    comboBoxDataBits->setCurrentIndex(3); //8 bit

    comboBoxParity->clear();
    comboBoxParity->addItem("None", QVariant::fromValue(PAR_NONE));
    comboBoxParity->addItem("Even", QVariant::fromValue(PAR_EVEN)); //pair
    comboBoxParity->addItem("Odd", QVariant::fromValue(PAR_ODD)); //impair
    comboBoxParity->addItem("Space", QVariant::fromValue(PAR_SPACE));
    comboBoxParity->setCurrentIndex(0); //Parity none

    comboBoxStopBits->clear();
    comboBoxStopBits->addItem("1", QVariant::fromValue(STOP_1));
    comboBoxStopBits->addItem("2", QVariant::fromValue(STOP_2));
    comboBoxStopBits->setCurrentIndex(0); //1 stop bit

    comboBoxFlowControl->clear();
    comboBoxFlowControl->addItem("None", QVariant::fromValue(FLOW_OFF));
    comboBoxFlowControl->addItem("Xon/Xoff", QVariant::fromValue(FLOW_XONXOFF));
    comboBoxFlowControl->addItem("Hardware", QVariant::fromValue(FLOW_HARDWARE));
    comboBoxFlowControl->setCurrentIndex(0); //Flow control none



    //this->installEventFilter(ui->textEdit_sendZone);
    //ui->textEdit_sendZone->installEventFilter(this);

    on_pushButton_clearCount_clicked();



    //DeviceInit();
}


//串口打开后进行初始化
void  MainWindow::DeviceInit(void) {
    //重启
    on_pushButton_restart_clicked();

    //设置Ap
    on_pushButton_setAp_clicked();
    //设置服务器的ip,端口
    on_pushButton_setServer_clicked();
    //设置服务器超时时间
    on_pushButton_setServerTimeout_clicked();

    //作为station链接Ap
    on_pushButton_connectAp_clicked();


    //定时去连接服务器
    //连接服务器
    on_pushButton_connectDstIp_clicked();
    //连接服务器
    //on_pushButton_connectDstIp_clicked();




}

MainWindow::~MainWindow()
{
    delete ui;
    delete serialPort;
    serialPort = NULL;
}








void MainWindow::DealRecDepCmd(void) {

    int resultStatus;

    delayDealStringTimer->stop();


    qDebug() << "deal string here";
    qDebug() << allRecString;
    int indexNum;
    if ((indexNum = allRecString.trimmed().toLower().indexOf("ok")) >= 0) {
        qDebug() << "ok here";
        resultStatus = 1;
        qDebug() << indexNum;

    } else if ((indexNum = allRecString.trimmed().toLower().indexOf("fail")) >= 0) {
        qDebug() << "fail";
        resultStatus = 2;

    } else if ((indexNum = allRecString.trimmed().toLower().indexOf("alreay connect")) >= 0) {
        qDebug() << "already connect here";
        resultStatus = 1;
        qDebug() << indexNum;

    } else {
        qDebug() << "error here!";
        resultStatus = 3;


    }

    SwitchCmd(resultStatus);



    allRecString = "";//处理完后清空
}

void MainWindow::SwitchCmd(int resultStatus) {
    if (nowCmd.indexOf("AT+CWJAP") >= 0) {
        if (resultStatus == 1) {
            ui->label_apStatus->setStyleSheet("background-color:green");
        } else  if (resultStatus == 2) {
            ui->label_apStatus->setStyleSheet("background-color:black");
        } else {
            ui->label_apStatus->setStyleSheet("background-color:red");
        }
    } else if (nowCmd.indexOf("AT+CIPSTART") >= 0) {
        if (resultStatus == 1) {
            ui->label_ipStatus->setStyleSheet("background-color:green");
        } else  if (resultStatus == 2) {
            ui->label_ipStatus->setStyleSheet("background-color:black");
        } else {
            ui->label_ipStatus->setStyleSheet("background-color:red");
        }
    } else if (nowCmd.indexOf(TCP_UDP_STATUS) >= 0) {
        if (allRecString.trimmed().toLower().indexOf("\r\nstatus:3") >= 0) {
            ui->label_ipStatus->setStyleSheet("background-color:green");
        } else {
            ui->label_ipStatus->setStyleSheet("background-color:black");
        }

    }

}


//一接受到字符就处理
void MainWindow::RawDealRecDepCmd(void) {
    int indexNum;

    if ((indexNum = allRecString.trimmed().toLower().indexOf("ok")) >= 0) {
        //qDebug() << "ok here";
        DealRecDepCmd();//再处理一次
    } else if ((indexNum = allRecString.trimmed().toLower().indexOf("fail")) >= 0) {
        //qDebug() << "fail";
        DealRecDepCmd();//再处理一次
    } else if ((indexNum = allRecString.trimmed().toLower().indexOf("alreay connect")) >= 0) {
        //qDebug() << "already connect here";
        DealRecDepCmd();//再处理一次

    }
}




void MainWindow::refreshCom(void) {

    m_listcomboName.clear();//清除上次检测的串口
    //查询注册表中可用的串口
    getCom();
    //
    int i;
    QString temp;
    QString showIn = "the following {";
    for (i = 0; i < m_listcomboName.size(); i++) {
        if (i != 0) {
            showIn += ", ";
        }
        qDebug() << m_listcomboName[i];
        temp = m_listcomboName[i];
        showIn += m_listcomboName[i];

    }
    showIn += "} can be used!";
    ui->textEdit_sendZone->setText(showIn);

    //qDebug() << temp.value<QString>();
    //serialPort->setPort(temp.value<QString>()); //Port
    int port_index = comboBoxPortName->findText(temp);
    qDebug("index is %d", port_index);
    comboBoxPortName->setCurrentIndex(port_index);


}

void MainWindow::on_pushButton_clearCount_clicked()
{
    //rxCount = 0;
    //txCount = 0;
    rxNum = 0;
    txNum = 0;
    ui->lineEdit_recSize->setText("RX:0");
    ui->lineEdit_txSize->setText("TX:0");

}






void MainWindow::keyPressEvent(QKeyEvent *e){


    //发送快捷键
    if (((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return)) && (e->modifiers() & Qt::ControlModifier))     {
        on_pushButton_manualSend_clicked();
    }
    //清除快捷键
    if (((e->key() == Qt::Key_L)) && (e->modifiers() & Qt::ControlModifier))     {
        on_pushButton_clearRec_clicked();
    }

    //打开串口快捷键
    if (((e->key() == Qt::Key_O)) && (e->modifiers() & Qt::ControlModifier))     {
        on_pushButton_openClose_clicked();
    }


    //打开串口快捷键
    if (((e->key() == Qt::Key_P)) && (e->modifiers() & Qt::ControlModifier))     {
        on_pushButton_checkCom_clicked();
    }
    //qDebug("%x", e->key());
    //qDebug() << "abc";
    //QApplication::keyboardModifiers () == Qt::ControlModifier

}

void MainWindow::autoScroll(void)
{
    QTextCursor cursor = ui->textEdit_recZone->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit_recZone->setTextCursor(cursor);
}
void MainWindow::on_pushButton_openClose_clicked()
{

    if (openStatus == false) {
        //直到开启串口才停止定时器
        autoOpenTimer->stop();

        //Settings parameters
        QVariant temp;

        temp = comboBoxPortName->itemData(comboBoxPortName->currentIndex());
        //        temp = comboBoxPortName->itemText(comboBoxPortName->currentIndex());
        //qDebug() << temp.toString();
        serialPort->setPort(temp.value<QString>()); //Port

        //QString s = temp.value<QString>();


        temp = comboBoxBaudRate->itemData(comboBoxBaudRate->currentIndex());
        //temp = QVariant::fromValue(14);
        //qDebug() << temp.value<BaudRateType>();
        serialPort->setBaudRate(temp.value<BaudRateType>()); //BaudRate

        temp = comboBoxDataBits->itemData(comboBoxDataBits->currentIndex());
        //qDebug() << temp.toString();
        serialPort->setDataBits(temp.value<DataBitsType>()); //DataBits

        temp = comboBoxParity->itemData(comboBoxParity->currentIndex());
        //qDebug() << temp.toString();
        serialPort->setParity(temp.value<ParityType>()); //Parity

        temp = comboBoxStopBits->itemData(comboBoxStopBits->currentIndex());
        //qDebug() << temp.toString();
        serialPort->setStopBits(temp.value<StopBitsType>()); //StopBits

        temp = comboBoxFlowControl->itemData(comboBoxFlowControl->currentIndex());
        //qDebug() << temp.toString();
        serialPort->setFlowControl(temp.value<FlowType>()); //FlowControl

        serialPort->setTimeout(0, 10);//10ms time out
        serialPort->enableSending();
        serialPort->enableReceiving();


        autoScroll();//避免焦点在前面
        if (serialPort->open() == true) {

            comStatus->setStyleSheet("background-color: green");
            qDebug() << "open ok";
            openStatus = true;
            ui->pushButton_openClose->setText("关闭串口");

            textEditReceive->setText("Open serial com OK!\r\n");


            //设备初始化
            DeviceInit();
        } else {
            comStatus->setStyleSheet(("background-color: red"));
            qDebug() << "open fail";
            textEditReceive->insertPlainText("Open serial com fail!\r\n");


        }


        serialPort->receiveData();

        textEditSend->setText("");
        //设置焦点在发送区
        textEditSend->setFocus();

        //是否启动定时发送
        if (ui->checkBox_autoSend->checkState() == Qt::Checked) {
            autoTimer->start();

        }
    } else {
        openStatus = false;

        serialPort->close();
        ui->pushButton_openClose->setText("打开串口");
        comStatus->setStyleSheet(("background-color: black"));
        qDebug() << "close";

        autoTimer->stop();
    }
}
void MainWindow::DealAfterSend(QByteArray dataReceived) {
    if (nowCmd.compare(lastCmd) == 0) {
        //qDebug() << nowCmd;
        //qDebug() << QString(dataReceived);
        //拼接接受到的字符串,定时时间到后再处理
        allRecString += QString(dataReceived);
        //直接处理拼接的字符
        RawDealRecDepCmd();

    } else {
        allRecString = "";//清空
        qDebug() << "not equal";
    }


    //必须拼接,执行新命令时清除
    int indexNum;
    otherRecString += QString(dataReceived);
    //其他主动发送数据的情况
    if ((indexNum = otherRecString.trimmed().toLower().indexOf("unlink")) >= 0) {
        ui->label_ipStatus->setStyleSheet("background-color: black");
    }


    if (nowCmd.trimmed().toLower().indexOf("at+rst") >= 0) {
        if (otherRecString.trimmed().toLower().indexOf("\r\nready") >= 0) {
            if (needToEmitFlag == 1) {
                qDebug() << "~~~send at rst here~~~~";
                otherRecString = "";
                emit ReceiveMisString();
            }

        }

    }
    //所有可能返回的字符串
    else if (otherRecString.trimmed().toLower().indexOf("ok") >= 0 ||
             otherRecString.trimmed().toLower().indexOf("no change") >= 0 ||
             otherRecString.trimmed().toLower().indexOf("\r\nmux=") >= 0 ||//为0或者1
             otherRecString.trimmed().toLower().indexOf("error") >= 0 ||
             otherRecString.trimmed().toLower().indexOf("fail") >= 0 ||
             otherRecString.trimmed().toLower().indexOf("link is not") >= 0 ||
             otherRecString.trimmed().toLower().indexOf("we must restart") >= 0 ||
             otherRecString.trimmed().toLower().indexOf("link is builded") >= 0 ||
             otherRecString.trimmed().toLower().indexOf("alreay connect") >= 0
             ) {


        QDateTime time = QDateTime::currentDateTime();
        qDebug() << "~~~~~~~~~send quit here~~~" +otherRecString + time.toLocalTime().toString("yyyy-MM-dd hh:mm:ss ddd");

        otherRecString = "";
        //::Sleep(1000);
        if (needToEmitFlag == 1) {
            emit ReceiveMisString();
        }
    }
    //

    //qDebug() << "tooooo";





    lastCmd = nowCmd;
}
void  MainWindow::slot_newDataReceived(const QByteArray &dataReceived)
{

    //串口关闭时不接受显示,避免应用程序无响应
    if (openStatus == false) {
        return;
    }
    unsigned char tmpBuf[4096];

    //自动往下移动  使输入往下
    autoScroll();
    //allDataReceived += dataReceived;

    //ui->textEdit_recZone->moveCursor(QTextCursor::End,  QTextCursor::MoveAnchor);

    //int mib = comboBoxCodecs->itemData(comboBoxCodecs->currentIndex()).toInt();
    //QTextCodec *codec = QTextCodec::codecForMib(0);
    //qDebug("size is %d", dataReceived.size());

    //QTextStream in(dataReceived);
    //in.setAutoDetectUnicode(false);
    //in.setCodec(codec);
    //QString decodedStr = QString((char*)tmpBuf);


    rxNum += dataReceived.size();
    //qDebug() << rxNum;
    ui->lineEdit_recSize->setText("RX:" + QString::number(rxNum));


    DealAfterSend(dataReceived);


    if (showFlag == true) {//是否显示接受到的数据

        if (ui->checkBox_hexShow->checkState() == Qt::Checked) {
            QString tmp;
            //char buf[4096] = {0};
            char buf1[20] = {0};
            for (int i = 0; i < dataReceived.size(); i++) {
                tmpBuf[i] = dataReceived.at(i) & 0xff;
                //qDebug("deal:%02x", tmpBuf[i]);

            }

            tmp = "";
            //sprintf(buf, "%s", decodedStr.toLatin1().data());
            //qDebug("result:%s", decodedStr.data());
            for (int i = 0; i < dataReceived.size(); i++) {
                sprintf(buf1, "%02X", tmpBuf[i]);//x控制大小写
                //qDebug("%s", buf1);
                tmp += QString(buf1) + " ";
            }

            textEditReceive->insertPlainText(tmp);
            //textEditReceive->insertPlainText("true");
        } else {
            textEditReceive->insertPlainText(QString(dataReceived));
            //textEditReceive->insertPlainText("false");
        }


        //自动清空显示
        if (ui->checkBox_autoClear->checkState() == Qt::Checked) {
            statisNum++;
            if (statisNum >= STATISTIX_MAX_NUM) {
                textEditReceive->setText("");
                statisNum = 0;
            }

        }
    } else {
        ;//不显示文本

    }




    //使显示往下
    autoScroll();
}

/*
bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    //Q_ASSERT(obj == ui->inputMsgEdit);

    if (e->type() == QEvent::KeyPress)
    {
        QKeyEvent *event = static_cast<QKeyEvent*>(e);
        if (event->key() == Qt::Key_Return && (event->modifiers() & Qt::ControlModifier))
        {
            //sendMessage();
            qDebug() << "zuhe";
            return true;
        }
    }
    qDebug() << "none";
}
*/
void MainWindow::on_pushButton_sendFile_clicked()
{

}

void MainWindow::on_pushButton_manualSend_clicked()
{

    if (openStatus == true) {
        QString Message;
        Message = textEditSend->toPlainText();

        if (ui->checkBox_hexSend->checkState() == Qt::Checked) {//十六进制发送
            QString tmp;
            char buf[4096] = {0};
            unsigned int buf1;
            unsigned char buf2[4096];
            tmp = "";
            Message = Message.replace(" ", "");
            sprintf(buf, "%s", Message.toLatin1().data());
            for (int i = 0; i < Message.size(); i+=2) {//"0234 56 78" = 0x12 0x 34
                sscanf(&buf[i], "%02x", &buf1);
                buf1 &= 0xff;
                //sprintf(buf2, "%cx%02X", "\\",buf1);//x控制大小写

                buf2[i/2] = buf1;
                //qDebug("%x %d", buf2[i/2], i);
                tmp += QString(buf2[i/2]);
            }
            Message = tmp;
        }

        //Message = "";
        //Retour a la ligne
        if (checkBoxCR->checkState() == Qt::Checked)
            Message += "\x0D";

        //Saut de ligne
        if (checkBoxLF->checkState() == Qt::Checked)
            Message += "\x0A";

        //Message += "\x0";
        QByteArray temp(Message.toLatin1());

        //发送区为空时返回
        if (temp.size() <= 0) {
            return;
        }

        //        unsigned char *p;
        //        p = (unsigned char *)temp.data();
        //        for (int i = 0; i < temp.size(); i++) {
        //            qDebug("send:%x", p[i]);
        //            //qDebug("send1:%x", temp.at(i));
        //        }
        AddToMsgEnd(temp.data(), DEFAULT_RECEIVE_TIMEOUT);
        ////RawSendData(temp);


        //如果需要发送完后自动清空发送区
        if (ui->checkBox_autoClearSend->checkState() == Qt::Checked) {
            textEditSend->setText("");
        }
        //qDebug() << "none";
    }
}

void MainWindow::RawSendData(QByteArray temp)
{


    nowCmd = QString(temp);

    delayDealStringTimer->stop();
    delayDealStringTimer->setInterval(delayDealStringTime);
    delayDealStringTimer->start();
    //发送新命令时，清除当前保存的数据
    allRecString = "";//清空
    //otherRecString = "";


    serialPort->sendData(temp);

    txNum += temp.size();
    ui->lineEdit_txSize->setText("TX:" + QString::number(txNum));

    textEditSend->setText(QString(temp));//加入"\r"无法写入
    //    //textEditSend->setText(QString(QByteArray::fromRawData(temp.data(), temp.size)));
    //    //textEditSend->setText("\r\n");
    //    textEditSend->set;
    //    temp = textEditSend->toPlainText().toLatin1();
    //    char *p = QString(temp).toLatin1().data();
    //    for (int i = 0; i < QString(temp).size(); i++) {
    //        qDebug("0x%x ", (unsigned char)p[i]);

    //    }


}

void MainWindow::on_pushButton_clearRec_clicked()
{
    textEditReceive->clear();//清空缓冲区
}

void MainWindow::on_pushButton_clearSend_clicked()
{
    textEditSend->clear();//清空发送区
}


//是否用回车作为发送标志
void MainWindow::on_textEdit_sendZone_textChanged()
{
    if(ui->textEdit_sendZone->toPlainText().endsWith("\n")){
        //qDebug() << "end here";
        if (ui->checkBox_returnAsSend->checkState() == Qt::Checked) {
            on_pushButton_manualSend_clicked();
        }
    }

}

void MainWindow::on_pushButton_stopShow_clicked()
{
    if (showFlag == true) {
        showFlag = false;
        ui->pushButton_stopShow->setText("继续显示");

        ui->label_stopShow->setStyleSheet("background-color: black");
    } else {
        showFlag = true;
        ui->pushButton_stopShow->setText("停止显示");
        ui->label_stopShow->setStyleSheet("background-color: green");
    }
}



void MainWindow::on_checkBox_autoSend_stateChanged(int arg1)
{
    //qDebug() << arg1;
    arg1 = arg1;
    if (ui->checkBox_autoSend->checkState() == Qt::Checked) {
        qDebug() << "true";
        int autoMs = ui->lineEdit_autoCircle->text().toInt();
        if (autoMs < INTERVAL_MIN) {
            autoMs = INTERVAL_MIN;  //最小10ms
            ui->lineEdit_autoCircle->setText(QString::number(autoMs));
        }
        autoTimer->setInterval(autoMs);
        autoTimer->start();
    } else {
        qDebug() << "false";
        autoTimer->stop();
    }
}

void MainWindow::on_lineEdit_autoCircle_textChanged(const QString &arg1)
{
    //qDebug() << arg1;
    //arg1 = arg1;
    QString tmp = arg1;

    delayTimer->setInterval(3000);//3s后启动
    delayTimer->start();

}
void MainWindow::DelaySetTimer(void) {

    int autoMs = ui->lineEdit_autoCircle->text().toInt();
    if (autoMs < INTERVAL_MIN) {
        autoMs = INTERVAL_MIN;  //最小10ms
        ui->lineEdit_autoCircle->setText(QString::number(autoMs));
    }
    autoTimer->setInterval(autoMs);
}

void MainWindow::on_pushButton_connectAp_clicked()
{
    char buf[100];
    char ssid[100];
    char pwd[100];

    //    ssid = ui->lineEdit_connectSSID->text().toLatin1().data();
    //    pwd = ui->lineEdit_connectPwd->text().toLatin1().data();
    strcpy(ssid, ui->lineEdit_connectSSID->text().toLatin1().data());
    strcpy(pwd, ui->lineEdit_connectPwd->text().toLatin1().data());
    //JoinNewAp(buf, JION_SSID, JION_PWD);
    JoinNewAp(buf, ssid, pwd);
    QByteArray tmp = QString(buf).toLatin1();//为1不是L

    delayDealStringTime = 10000;//10s
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    ////RawSendData(tmp);
}

void MainWindow::on_pushButton_restart_clicked()
{

    QByteArray tmp = QString(RESTART_DEVICE).toLatin1();//为1不是L
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    ////RawSendData(tmp);
}

void MainWindow::on_pushButton_getCrc32_clicked()
{
    char s[100];
    char buf[50];
    strcpy(s, ui->lineEdit_crc32->text().toLatin1().data());

    sprintf(buf, "0X%08X", crc32(0, s, strlen(s)));
    //qDebug("0X%08X", crc32(0, s, strlen(s)));
    ui->lineEdit_getCrc32->setText(QString(buf));
}

void MainWindow::on_pushButton_getMd5_clicked()
{
    unsigned char digest[16];  //存放结果
    unsigned int i;
    char s[100];
    char buf[3];
    QString tmp;

    strcpy(s, ui->lineEdit_md5->text().toLatin1().data());

    MDString(s,digest); //直接输入字符串并得出结果
    for (i = 0; i < 16; i++) {
        sprintf(buf, "%02X", digest[i]);
        //printf("%x ", digest[i]);
        tmp += QString(buf);
    }
    ui->lineEdit_getMd5->setText(tmp);
}

void MainWindow::firstInit(void) {
    on_pushButton_getCrc32_clicked();
    on_pushButton_getMd5_clicked();


}



void MainWindow::getCom(void) {

    QString path = "HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM\\";
    QSettings *settings = new QSettings(path, QSettings::NativeFormat);

    QStringList key = settings->allKeys();
    int num = (int)key.size();
    QString value;


    for(int i = 0; i < num; i++)
    {
        value = rawGetCom(i, "value");
        m_listcomboName << value;
        qDebug() << "value is " + value;
    }
    //qDebug() << m_listcomboName[0];

}
QString MainWindow::rawGetCom(int index,QString keyorvalue)
//index为索引号，keyorvalue为选择返回值应该为key还是value
{
    QString commresult;

    if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),0,KEY_READ,&hKey)!=0)
    {
        QString error="Cannot open regedit!";//无法打开注册表时返回error
        return error;
    }//if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_READ,&hKey)!=0)
    QString keymessage;//键名
    QString message;
    QString valuemessage;//键值
    indexnum = index;//要读取键值的索引号
    keysize=sizeof(keyname);
    valuesize=sizeof(keyvalue);
    if(::RegEnumValue(hKey,indexnum,keyname,&keysize,0,&type,(BYTE*)keyvalue,&valuesize)==0)//列举键名和值
    {
        for(unsigned int i=0;i<keysize;i++)
        {
            message=keyname[i];
            keymessage.append(message);
        }// for(int i=0;i<=keysize;i++)    读取键名
        for(unsigned int j=0;j<valuesize;j++)
        {
            if(keyvalue[j]!=0x00)
            {
                valuemessage.append(keyvalue[j]);
            }
        }//for(int j=0;j<valuesize;j++) 读取键值
        if(keyorvalue=="key")
        {
            commresult=keymessage;
        }
        if(keyorvalue=="value")
        {
            commresult=valuemessage;
        }
    }
    else
    {
        commresult="nokey";
    }
    ::RegCloseKey(hKey);//关闭注册表

    //if(::RegEnumValue(hKey,indexnum,keyname,&keysize,0,&type,(BYTE*)keyvalue,&valuesize)==0) 列举键名和值
    return commresult;
}





void MainWindow::on_pushButton_checkCom_clicked()
{
    refreshCom();

    //一秒后自动打开串口
    autoOpenTimer->start();

}

void MainWindow::on_pushButton_help_clicked()
{
    ui->textEdit_recZone->setText("");
    ui->textEdit_recZone->insertPlainText("\t\t软件使用说明\r\n\r\n\r\n");
    ui->textEdit_recZone->insertPlainText("CTRL+O----------打开关闭串口\r\n");
    ui->textEdit_recZone->insertPlainText("CTRL+L----------清除显示\r\n");
    ui->textEdit_recZone->insertPlainText("CTRL+P----------检测端口\r\n");
    ui->textEdit_recZone->insertPlainText("CTRL+ENTER------发送数据\r\n");
    ui->textEdit_recZone->insertPlainText("点击“检测端口”可检测当前可用串口号\r\n");
    ui->textEdit_recZone->insertPlainText("自动发送周期最少为10ms\r\n");
    ui->textEdit_recZone->insertPlainText("打开串口后自动将焦点至于发送区\r\n");
    ui->textEdit_recZone->insertPlainText("选上自动发送，选上发送完自动清空，并设置自动发送周期为10ms，可以达到实时发送的要求\r\n");
    ui->textEdit_recZone->insertPlainText("\r\n\r\n\r\n\r\n\r\n\r\n\t\t\t\t\tauthor:haozi\r\n");
    ui->textEdit_recZone->insertPlainText("\t\t\t\t\tEmail:haozi_winged@163.com\r\n");
    ui->textEdit_recZone->insertPlainText("\t\t\t\t\tData:2015/02/17\r\n");
    //ui->textEdit_recZone->insertPlainText("");


}

void MainWindow::on_pushButton_listAp_clicked()
{
    delayDealStringTime = 8000;//延时8s后再统一处理接收到的数据
    QByteArray tmp = QString(LIST_CURRENT_AP).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    ////RawSendData(tmp);
}

void MainWindow::on_pushButton_querySelfIp_clicked()
{
    QByteArray tmp = QString(GET_SELF_IP).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_currentMode_clicked()
{
    QByteArray tmp = QString(QUERY_MODE).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_queryConnectAp_clicked()
{
    QByteArray tmp = QString(JOIN_WHICH_AP).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_listConnectIp_clicked()
{
    QByteArray tmp = QString(LIST_CONNECT_IP).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_setApMode_clicked()
{
    QByteArray tmp = QString(SETMODE_AP).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_setStation_clicked()
{
    QByteArray tmp = QString(SETMODE_STATION).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_setApStation_clicked()
{
    QByteArray tmp = QString(SETMODE_AP_STATION).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);

}
//当mux=0时才有效，作为服务器mux=1时，该作用无效
void MainWindow::on_pushButton_closeTcp_clicked()
{

    QByteArray tmp = QString(TCP_UDP_CLOSE).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
    ui->label_ipStatus->setStyleSheet("background-color: black");
}



//作为station去连接服务器时  AT+CIPMUX=0，mux需要等于0才能连上
void MainWindow::on_pushButton_connectDstIp_clicked()
{

    char buf[100];
    char connectIp[100];
    int serverPort;



    //必须在mux=0的情况下
    QByteArray tmp = QString(MUL_LINK0).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);

    //::Sleep(2*1000);//单位毫秒


    //connectIp = ui->lineEdit_dstIp->text().toLatin1().data();
    strcpy(connectIp, ui->lineEdit_dstIp->text().toLatin1().data());
    serverPort = ui->lineEdit_dstPort->text().toInt();
    //TcpConnectServer(buf, CONNECT_SERVER_IP, SERVER_PORT);
    TcpConnectServer(buf, connectIp, serverPort);
    tmp = QString(buf).toLatin1();//为1不是L
    delayDealStringTime = 5000;
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);

}

void MainWindow::on_pushButton_checkTcpUdpStatus_clicked()
{
    QByteArray tmp = QString(TCP_UDP_STATUS).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}


//设置完后需要重启
void MainWindow::on_pushButton_setAp_clicked()
{

    char buf[100];
    char apSsid[100];
    char apPwd[100];
    int apPwdMode;

    strcpy(apSsid, ui->lineEdit_apSsid->text().toLatin1().data());
    //apSsid = ui->lineEdit_apSsid->text().toLatin1().data();
    //apPwd = ui->lineEdit_apPwd->text().toLatin1().data();
    strcpy(apPwd, ui->lineEdit_apPwd->text().toLatin1().data());
    apPwdMode = ui->comboBox_apPwdMode->currentIndex();
    //SetNewAp(buf, SET_AP_SSID, SET_AP_PWD, SET_PWD_MODE, SET_CHANNEL);
    SetNewAp(buf, apSsid, apPwd, apPwdMode, SET_CHANNEL);
    QByteArray tmp = QString(buf).toLatin1();//为1不是L

    delayDealStringTime = 5000;
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_setServer_clicked()
{
    char buf[100];
    char *setSelfIp;
    unsigned int setSelfPort;
    //需要设置超时，否则一直占用连接，导致其他命令不能用
    //AT+CIPSTO=1//单位秒


    //设置多路链接  单路链接为0也可以？设置完后重新设为0?
    QByteArray tmp = QString(MUL_LINK1).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);

    //::Sleep(2*1000);//单位毫秒


    setSelfIp = ui->lineEdit_setSelfIp->text().toLatin1().data();
    setSelfPort = ui->lineEdit_setSelfPort->text().toInt();
    SetServerPort(buf, setSelfIp, setSelfPort);
    tmp = QString(buf).toLatin1();
    delayDealStringTime = 5000;
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);


    //设置AT+CIPMUX=1到0后，服务器和客户端同时存在
}

void MainWindow::on_pushButton_testSend_clicked()
{
    //char buf[100];
    char str[100];

    QByteArray tmp;

    //设置要发送的数据大小及哪个连接
    //int linkId = ui->comboBox_linkId->currentIndex();
    //str = ui->lineEdit_testSend->text().toLatin1().data();
    strcpy(str, ui->lineEdit_testSend->text().toLatin1().data());

    //ServerPassSendString(buf, linkId, str);
    //qDebug() << buf;
    //tmp = QString(buf).toLatin1();


    /*延时1s后发送,等待出现">"时再发送*/
    //::Sleep(1*1000);//单位毫秒

    //发送数据
    tmp = QString(str).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);


}

void MainWindow::on_pushButton_sendNum_clicked()
{
    char buf[100];
    char str[100];


    //设置要发送的数据大小及哪个连接
    int linkId = ui->comboBox_linkId->currentIndex();
    //str = ui->lineEdit_testSend->text().toLatin1().data();
    strcpy(str, ui->lineEdit_testSend->text().toLatin1().data());
    ServerPassSendString(buf, linkId, str);
    qDebug() << buf;
    QByteArray tmp = QString(buf).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_setServerTimeout_clicked()
{
    unsigned int timeOut;
    char buf[100];

    timeOut = ui->lineEdit_serverTimeout->text().toInt();
    //AT+CIPSTO=1//单位秒
    sprintf(buf, "AT+CIPSTO=%d\r\n", timeOut);

    QByteArray tmp = QString(buf).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_sendNumSingle_clicked()
{
    char buf[100];
    char str[100];

    //char *str = ui->lineEdit_testSendSingle->text().toLatin1().data();
    strcpy(str, ui->lineEdit_testSendSingle->text().toLatin1().data());
    NonePassSendString(buf, str);
    QByteArray tmp = QString(buf).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);

}

void MainWindow::on_pushButton_inqueryServerTimeout_clicked()
{
    char buf[100];

    //timeOut = ui->lineEdit_serverTimeout->text().toInt();
    //AT+CIPSTO=1//单位秒
    sprintf(buf, "AT+CIPSTO?\r\n");

    QByteArray tmp = QString(buf).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_closeTcpMux_clicked()
{
    int indexNum;
    char buf[100];


    indexNum = ui->comboBox_linkId->currentIndex();
    sprintf(buf, "AT+CIPCLOSE=%d\r\n", indexNum);


    QByteArray tmp = QString(buf).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_setSingleLink_clicked()
{
    QByteArray tmp = QString(MUL_LINK0).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_setMuxLink_clicked()
{
    QByteArray tmp = QString(MUL_LINK1).toLatin1();
    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);

}

void MainWindow::on_pushButton_transparentOrNone_clicked()
{
    QByteArray tmp;

    if (transparentMode == false) {
        transparentMode = true;
        // 设置透传模式
        tmp = QString(PASS_THROUGH_MODE).toLatin1();
        ui->pushButton_transparentOrNone->setText("非透传模式");
    } else {
        tmp = QString(NONE_PASS_THROUGH_MODE).toLatin1();
        transparentMode = false;
        ui->pushButton_transparentOrNone->setText("透传模式");
    }

    AddToMsgEnd(tmp.data(), DEFAULT_RECEIVE_TIMEOUT);
    //RawSendData(tmp);
}

void MainWindow::on_pushButton_sendSignal_clicked()
{
    emit ReceiveMisString();
}
