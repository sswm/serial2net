#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client/client.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void CommonSendLed();

public:
    client *cli;
    QString rec;
    QByteArray recordTmp;

    unsigned char ledStatus;

private slots:

    void on_pushButton_openLed1_clicked();

    void on_pushButton_openLed2_clicked();

    void on_pushButton_openLed3_clicked();

    void on_pushButton_openLed4_clicked();

    void on_pushButton_openLed5_clicked();



    void on_pushButton_sendStr_clicked();

    void on_pushButton_connect_clicked();

    void ShowReceiveStr(QByteArray rec);


    void on_pushButton_clearZone_clicked();

    void on_pushButton_closeTcp_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
