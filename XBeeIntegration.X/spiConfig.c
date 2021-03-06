#include <xc.h>

//Accel and Gyro use SPI1 with the setting 0x0320
void SPI1Init(void) {
    SPI1CON1 = 0x0260; // Master Mode, 8-bit bytes, Idle state low, Active Hi
    // Data changes on clock transition from Active to Idle
    // SCL1 at 16000000/(8*64) = 31.25 kHz.
    SPI1STAT = 0x8000; // enable SPI peripheral
}// SPI1Init

// Send one byte of data and receive one back at the same time

unsigned char writeSPI1(unsigned char j) {
    while (SPI1STATbits.SPITBF); // If SPI1TBF full, just wait. This may not
    // be necessary, but good practice.
    SPI1BUF = j; // Write byte to SPI1BUF
    while (!SPI1STATbits.SPIRBF); // Wait if Receive Not Complete
    return SPI1BUF; // Read the received value
}// writeSPI1

unsigned char readSPI1(void) {
    while (SPI1STATbits.SPITBF); // If SPI1TBF full, just wait. This may not
    // be necessary, but good practice.
    SPI1BUF = 0x3D; // Write byte to SPI1BUF
    while (!SPI1STATbits.SPIRBF); // Wait if Receive Not Complete
    return SPI1BUF; // Read the received value
}// writeSPI1

//Bar pressure & Xbee uses SPI2 with the setting 0x0320
void SPI2Init(void) {
    SPI2CON1 = 0x0120; // Master Mode, 8-bit bytes, Idle state low, Active Hi
    // Data changes on clock transition from Active to Idle
    // SCL1 at 16000000/(8*64) = 31.25 kHz.
    SPI2STAT = 0x8000; // enable SPI peripheral
}// SPI2Init

// Send one byte of data and receive one back at the same time

unsigned char writeSPI2(unsigned char j) {
    while (SPI2STATbits.SPITBF); // If SPI1TBF full, just wait. This may not
    // be necessary, but good practice.
    SPI2BUF = j; // Write byte to SPI1BUF
    while (!SPI2STATbits.SPIRBF); // Wait if Receive Not Complete
    return SPI2BUF; // Read the received value
}// writeSPI2

unsigned char readSPI2(void) {
    while (SPI2STATbits.SPITBF); // If SPI1TBF full, just wait. This may not
    // be necessary, but good practice.
    SPI2BUF = 0x3D; // Write byte to SPI1BUF
    while (!SPI2STATbits.SPIRBF); // Wait if Receive Not Complete
    return SPI2BUF; // Read the received value
}// writeSPI2

void us_delay(int us)   // Max value 32767 us = 0.032767s, min division 
{
    T2CON = 0x8010; // Timer 2 on, TCKPS<1,0> = 01 thus 8 Prescale thus 2MHz
    TMR2 = 0;
    while (TMR2 < us * 2); // 1/16MHz/(8*2)) = 1us.)
}

void ms_delay(int ms)   // Max value 1048 ms = 1.048s, min division
{
    T2CON = 0x8030; // Timer 2 on, TCKPS<1,0> = 11 thus 256 Prescale
    TMR2 = 0;
    while (TMR2 < ms * 63); // 1/16MHz/(256*63)) = 0.001008 close to 1 ms.)
}
