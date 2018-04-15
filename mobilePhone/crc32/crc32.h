#ifndef __CRC32_H
#define __CRC32_H



#ifdef   __cplusplus
extern   "C"
{
#endif



extern unsigned int POLYNOMIAL;
extern unsigned char have_table;
extern unsigned int table[256];

void make_table(void);
unsigned int crc32(unsigned int crc, char *buff, int len);



#ifdef   __cplusplus
}

#endif


#endif
