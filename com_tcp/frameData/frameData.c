#include "frameData.h"
#include "crc32/crc32.h"
#include <string.h>
#include <malloc.h>

char receiveData[256];
/*
*size:总数

*/
unsigned char CheckFrameDataCrc(char *buf, int size, frameData *smallData) {
    unsigned int crc;

    if ((unsigned char)buf[0] != TRANSPARENT_CMD) {
        return -1;
    }
    smallData->cmdHead = buf[0];

    smallData->dataLen = buf[1];

    smallData->selfIp = (buf[2] << 8) | (buf[3]);//0x00 02
    smallData->dstIp = (buf[4] << 8) | (buf[5]);//0x00 01


    smallData->p = receiveData;
    memcpy(smallData->p, &buf[6],size - 10);
    smallData->p[size - 10] = 0;


    smallData->crc = crc32(0, buf, size - 4);

    crc = (0xff & buf[size - 4]) << 24;
    crc |= (0xff & buf[size - 3]) << 16;
    crc |= (0xff & buf[size - 2]) << 8;
    crc |= (0xff & buf[size - 1]);
    printf("calc is %x    crc:%x\r\n", smallData->crc, crc);
    if (smallData->crc != crc) {

        return -1;
    }
    return 0;
}


unsigned char DealFrameData(char *buf, int *size) {

    char *allData;
    frameData   smallData;

    smallData.cmdHead = TRANSPARENT_CMD;
    smallData.dataLen = 10 + *size;
    smallData.dstIp = DST_IP;//0x00 01
    smallData.selfIp = SELF_IP;
    smallData.p = buf;

    allData = (char *)malloc((unsigned int)smallData.dataLen);
    if (allData == 0) {
        return -1;

    }
    allData[0] = smallData.cmdHead;
    allData[1] = smallData.dataLen;

    allData[2] = 0xff & (smallData.dstIp >> 8);
    allData[3] = 0xff & (smallData.dstIp);

    allData[4] = 0xff & (smallData.selfIp >> 8);
    allData[5] = 0xff & (smallData.selfIp);

    memcpy(&allData[6], smallData.p, *size);


    smallData.crc = crc32(0, allData, *size + 6);
    allData[*size + 6] = 0xff & (smallData.crc >> 24);
    allData[*size + 7] = 0xff & (smallData.crc >> 16);
    allData[*size + 8] = 0xff & (smallData.crc >> 8);
    allData[*size + 9] = 0xff & (smallData.crc >> 0);


    *size = *size + 10;
    memcpy(buf, allData, *size);

    free(allData);

    return 0;
}
