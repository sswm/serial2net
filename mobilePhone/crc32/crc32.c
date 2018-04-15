# include <stdio.h>
# include <string.h>

unsigned int POLYNOMIAL = 0xEDB88320;
unsigned char have_table = 0;
unsigned int table[256];



void make_table()
{
    int i, j;
    have_table = 1 ;
    for (i = 0 ; i < 256 ; i++)
        for (j = 0, table[i] = i ; j < 8 ; j++)
            table[i] = (table[i]>>1)^((table[i]&1)?POLYNOMIAL:0) ;
}


unsigned int crc32(unsigned int crc, char *buff, int len)
{
    int i;
    if (!have_table) make_table() ;
    crc = ~crc;
    for (i = 0; i < len; i++)
        crc = (crc >> 8) ^ table[(crc ^ buff[i]) & 0xff];
    return ~crc;
}

/*
int main ()
{
    char s[] = "12345";
    printf("0X%08x\n", crc32(0, s, strlen(s)));
    return 0 ;
}
*/
