#ifndef __FRAME_DATA_H
#define __FRAME_DATA_H







#ifdef   __cplusplus
extern   "C"
{
#endif



//传输命令
#define     TRANSPARENT_CMD         0x31
//传输数据的大小
#define     FRAME_SIZE              0xc8   //=200
//目标地址
#define     DST_IP                  0x1
//自身地址
#define     SELF_IP                 0x2

//起始标示
#define     DATA_HEAD               0xfd


#define     SMALL_SIZE              20


typedef struct oneFrame {
    unsigned char cmdHead;
    unsigned char dataLen;//数据总长度
    unsigned short dstIp;//2
    unsigned short selfIp;//2
    char *p;//数据指针
    unsigned int crc;//4
} frameData;








unsigned char DealFrameData(char *buf, int *size);

unsigned char CheckFrameDataCrc(char *buf, int size, frameData *smallData);








#ifdef   __cplusplus
}

#endif


#endif
