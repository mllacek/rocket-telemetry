#ifndef spiConfiguration
#define	spiConfiguration
void SPI1Init(void);
unsigned char writeSPI1(unsigned char j);
unsigned char readSPI1(void);
void SPI2Init(void);
unsigned char writeSPI2(unsigned char j);
unsigned char readSPI2(void);
void us_delay(int us);
void ms_delay(int ms);
#endif