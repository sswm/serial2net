#ifndef QSENDSERIAL_H
#define QSENDSERIAL_H


#include <QThread>
#include <QProcess>
#include <QTimer>


class QSendSerial : public QThread
{
    Q_OBJECT
public:
    QSendSerial();
    ~QSendSerial();
    void run();

    void ThreadInit();

public:
    QTimer *receiveTimeoutTimer;

signals:
    void TryToSendData(QByteArray tmp);

public slots:
    void SendTimeoutSignal();

};

#endif // QSENDSERIAL_H
