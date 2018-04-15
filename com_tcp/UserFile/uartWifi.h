#ifndef __UARTWIFI_H
#define __UARTWIFI_H



//失败后的测试次数
#define		TRY_SEND_TIMES		5



#define		QUERY_MODE			"AT+CWMODE?\r\n"
#define		SETMODE_STATION		"AT+CWMODE=1\r\n"
#define		SETMODE_AP			"AT+CWMODE=2\r\n"
#define		SETMODE_AP_STATION	"AT+CWMODE=3\r\n"


#define		RESTART_DEVICE			"AT+RST\r\n"

#define		LIST_CURRENT_AP			"AT+CWLAP\r\n"


//station模式
#define		JION_SSID				"TP-LINK_9319A4"


#define		JION_PWD				"wuxian640212wuxian"

#define		QUIT_NOW_AP				"AT+CWQAP\r\n"

#define		JOIN_WHICH_AP			"AT+CWJAP?\r\n"


#define		TCP_UDP_STATUS			"AT+CIPSTATUS\r\n"
#define		LIST_CONNECT_IP			"AT+CWLIF\r\n"





//非透传

#define		NONE_PASS_THROUGH_MODE	"AT+CIPMODE=0\r\n"

//透传
#define		PASS_THROUGH_MODE	"AT+CIPMODE=1\r\n"

#define 	TCP_UDP_CLOSE		"AT+CIPCLOSE\r\n"



#define		CONNECT_SERVER_IP		"192.168.1.105"

#define		SERVER_PORT				4321





//ap模式
#define		SET_AP_SSID				"MyUartWifi2"
#define		SET_AP_PWD				"c12345678"


#define		MUL_LINK0				"AT+CIPMUX=0\r\n"

#define		MUL_LINK1				"AT+CIPMUX=1\r\n"



#define	BUF_SIZE	1024
#define	END_REC_TIME		100MS

#define	BUSY_STR		"busy p..."

#define	CWLAP_FLAG_STR	"+CWLAP:"

#define	COMPARE_MODE_STR	"+CWMODE:"


//通道为0或者1时不用输入密码
#define		SET_CHANNEL				2




//ip
//获取本地ip
#define		GET_SELF_IP       			"AT+CIFSR\r\n"




typedef enum {
	AP_MODE_OPEN = 0,
	AP_MODE_WEP = 1,
	AP_MODE_WPA_PSK = 2,
	AP_MODE_WPA2_PSK = 3,
	AP_MODE_WPA_WPA2_PSK = 4

} AP_PWD_MODE;

#define		SET_PWD_MODE			AP_MODE_WPA_WPA2_PSK







typedef enum {
	GetError = 0,
	GetGood = !GetError

} Result;




/*
struct msg
{
    int start;
    int tempe;//温度
    int humidity;//湿度
    //unsigned char temp[2];
    //unsigned char hum[2];
    int light;//光照
    int led_state;//led状态
    int fan_state;//风扇状态
    int beep_state;//蜂鸣器状态
    int x;
    int y;
    int z;
    int alarm_tempe;
    int end;
    //校验
};

*/


#ifdef   __cplusplus
extern   "C"
{
#endif

//外部发送函数
extern void externRawWrite(char *cmd, unsigned int len, unsigned int waitTime);
extern void externWinSleep(unsigned int secs);


char *SetServerPort(char *buf, char *ip, unsigned int port);


char *JoinNewAp(char *buf, char *ssid, char *pwd);


char *SetNewAp(char *buf, char *ssid, char *pwd, unsigned char pwdmode, unsigned char channel);
char *TcpConnectServer(char *buf, char *ip, unsigned int port);

char *NonePassSendString(char *buf, char *str);

char *ServerPassSendString(char *buf, int linkId, int size);



void CountAp(char *str);



#ifdef   __cplusplus
}

#endif




#endif 
