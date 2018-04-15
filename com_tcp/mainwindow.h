#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include  "SerialPort/ManageSerialPort.h"
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QTextEdit>
#include <QTimer>
#include <QSettings>
#include "UserFile/uartWifi.h"
#include "link/list.h"



#define     DEFAULT_RECEIVE_TIMEOUT  20

#define     DEFAULT_DEALY           3000//默认处理命令后多长时间处理接受到的字符串

#define     STATISTIX_MAX_NUM       500//大于2000次时清楚显示


#define         INTERVAL_MIN        10










#define         ONE_FRAME_SIZE      200


extern QMutex   linkMutex;



typedef struct postMsg {
    unsigned int timeout;
    char buf[ONE_FRAME_SIZE];
    int bufSize;
    struct list_head list;

}postLinked;




//






namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void autoScroll(void);
    //virtual bool eventFilter(QObject *obj, QEvent *e);
    void firstInit(void);
    void getCom(void);
    QString rawGetCom(int index,QString keyorvalue);
    void refreshCom(void);

    void DealAfterSend(QByteArray dataReceived);
    void RawDealRecDepCmd(void);
    void SwitchCmd(int resultStatus);
    int AddToMsgEnd(char *str, int size, unsigned int timeout);
    int GetFromMsgStart(char *str, int *timeout, int *size);
    void DeviceInit(void);
    void RawDealFrameData(QByteArray otherRecString);
    void ReturnStringToClient(char *buf, unsigned int size, int linkId);
    void ControlFrameData(char *p, int len);
    void DealLedHere(unsigned char *p);
public:

    QString nowCmd;//当前发送给串口的命令
    QString lastCmd;


    QString rxCount;
    QString txCount;
    unsigned int rxNum;
    unsigned int txNum;
    unsigned int statisNum;
    ManageSerialPort *serialPort;
    //QByteArray allDataReceived;

    QTimer  *showTimer;

    //自动发送等相关定时器
    QTimer  *autoTimer;
    QTimer  *delayTimer;//延时处理设置的定时时常

    QTimer  *autoOpenTimer;//自动打开串口定时器

    QTimer  *delayDealStringTimer;//发送命令延时处理定时器
    unsigned int delayDealStringTime;//发送命令延时时间
    QString allRecString;

    QByteArray otherRecString;



    QStringList m_listcomboName;

    HKEY hKey;
    LPCWSTR subkey;
    wchar_t keyname[256]; //键名数组
    char keyvalue[256];  //键值数组
    DWORD keysize,type,valuesize;
    int indexnum;



    QComboBox *comboBoxPortName;//prot
    QComboBox *comboBoxBaudRate;//baudrate
    QComboBox *comboBoxParity;//crc
    QComboBox *comboBoxDataBits;//
    QComboBox *comboBoxStopBits;
    QComboBox *comboBoxFlowControl;

    QTextEdit *textEditSend;
    QTextEdit *textEditReceive;

    QCheckBox *checkBoxCR;
    QCheckBox *checkBoxLF;

    QLabel *comStatus;

    bool openStatus;
    bool showFlag;

    bool transparentMode;
    //bool autoFlag;
signals:
    void ReceiveMisString();

public slots:

    void RawSendData(QByteArray temp);
    void on_pushButton_clearCount_clicked();

    void on_pushButton_openClose_clicked();
    //SerialPort
    void slot_newDataReceived(const QByteArray dataReceived);
    void on_pushButton_sendFile_clicked();

    void on_pushButton_manualSend_clicked();

    void on_pushButton_clearRec_clicked();

    void on_pushButton_clearSend_clicked();
    void keyPressEvent(QKeyEvent *e);

    void on_textEdit_sendZone_textChanged();

    void on_pushButton_stopShow_clicked();

    void on_checkBox_autoSend_stateChanged(int arg1);

    void on_lineEdit_autoCircle_textChanged(const QString &arg1);

    void DelaySetTimer(void);

    void on_pushButton_connectAp_clicked();

    void on_pushButton_restart_clicked();

    void on_pushButton_getCrc32_clicked();

    void on_pushButton_getMd5_clicked();

    void on_pushButton_checkCom_clicked();

    void on_pushButton_help_clicked();

    void on_pushButton_listAp_clicked();

    void on_pushButton_querySelfIp_clicked();

    void on_pushButton_currentMode_clicked();

    void on_pushButton_queryConnectAp_clicked();

    void on_pushButton_listConnectIp_clicked();

    void on_pushButton_setApMode_clicked();

    void on_pushButton_setStation_clicked();

    void on_pushButton_setApStation_clicked();

    void DealRecDepCmd();

    void on_pushButton_closeTcp_clicked();

    void on_pushButton_connectDstIp_clicked();

    void on_pushButton_checkTcpUdpStatus_clicked();

    void on_pushButton_setAp_clicked();

    void on_pushButton_setServer_clicked();

    void on_pushButton_testSend_clicked();

    void on_pushButton_sendNum_clicked();

    void on_pushButton_setServerTimeout_clicked();

    void on_pushButton_sendNumSingle_clicked();

    void on_pushButton_inqueryServerTimeout_clicked();

    void on_pushButton_closeTcpMux_clicked();

    void on_pushButton_setSingleLink_clicked();

    void on_pushButton_setMuxLink_clicked();

    void ShowSystemTime();

    void on_pushButton_transparentOrNone_clicked();
    void SendFrameData(char *buf, int sendSize, int linkId);

    void on_pushButton_sendSignal_clicked();

    void on_pushButton_sendXieYi_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
