/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */


#ifndef spiConfiguration
#define	spiConfiguration
void SPI1Init(void);
unsigned char writeSPI1(unsigned char j);
unsigned char readSPI1(void);
void us_delay(int us);
void ms_delay(int ms);
#endif