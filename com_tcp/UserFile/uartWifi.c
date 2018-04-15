
#include "uartWifi.h"
#include <stdio.h>
#include <string.h>
//#include <windows.h>





unsigned char recTimeOut;
unsigned char recOkFlag;

unsigned char dealString[2048];




//发送完命令后等待多长时间
//unsigned int waitRecTime = 1;







char *SetServerPort(char *buf, char *ip, unsigned int port) {
    if(ip == NULL) {
        return GetError;
    }

    sprintf(buf, "AT+CIPSERVER=1,%d\r\n", port);

    return buf;
}

char *JoinNewAp(char *buf, char *ssid, char *pwd) {



    if((ssid == NULL) || (pwd == NULL) || (strlen(ssid) == 0) || (strlen(pwd) == 0)) {
        return GetError;
    }

    //NowCommand(SETMODE_STATION, 1);
    sprintf(buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    //printf("%s", buf);
    //NowCommand(buf, 5);
    //externWinSleep(1);
    //NowCommand(RESTART_DEVICE);
    return buf;
}










char *SetNewAp(char *buf, char *ssid, char *pwd, unsigned char pwdmode, unsigned char channel) {


    if((ssid == NULL) || (pwd == NULL) || (strlen(ssid) == 0) || (strlen(pwd) == 0)) {
        return GetError;
    }

    //AT+CWSAP="mywifi","12345678",1,0
    sprintf(buf, "AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n", ssid, pwd, pwdmode, channel);
    //printf("%s", buf);
    //NowCommand(buf, 1);
    //externWinSleep(1);//信号量来控制
    //NowCommand(RESTART_DEVICE, 1);

    return buf;
}




char *TcpConnectServer(char *buf, char *ip, unsigned int port) {



    if((ip == NULL)|| (strlen(ip) == 0)) {
        return GetError;
    }

    //AT+CIPSTART="TCP","192.168.1.110",4321
    sprintf(buf, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ip, port);

    //NowCommand(buf, 1);

    return buf;
}


char *NonePassSendString(char *buf, char *str) {

    if((str == NULL)|| (strlen(str) == 0)) {
        return GetError;
    }

    //AT+CIPSTART="TCP","192.168.1.110",4321
    sprintf(buf, "AT+CIPSEND=%d\r\n%s", strlen(str), str);


    return buf;
}


char *ServerPassSendString(char *buf, int linkId, char *str) {


    if((str == NULL)|| (strlen(str) == 0)) {
        return GetError;
    }


    //NowCommand("AT+CIPSEND=0,5\r\n");
    sprintf(buf, "AT+CIPSEND=%d,%d\r\n", linkId, strlen(str));

    return buf;
}





void CountAp(char *str) {
    unsigned int i;
    char *p;
    char *q;

    i = 0;
    p = str;
    while((q = strstr(p, CWLAP_FLAG_STR)) != NULL) {
        i++;
        p = q + strlen(CWLAP_FLAG_STR);
        //printf("~~%s~%d~\n", p, i);
    }
    printf("ssid num is %d\n", i);


}



