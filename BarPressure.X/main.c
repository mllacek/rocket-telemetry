/*  Rocket Telemetry Senior Design Project
    Author: David Dalvin and Moncia Lacek
    Created on 1/22/2019
    Version 1.0
    SPI Code to Interface with Bar. pressure sensor
 */

#include "config.h"
#include <libpic30.h>
#include <math.h>
#include <stdio.h>

int registerStatus;

unsigned int SENS_T1 = 0; //pressure sensitivity C1
unsigned int OFF_T1= 0; //pressure offset, C2
unsigned int TCS= 0; //temp coefficient of pressure sensitivity C3
unsigned int TCO= 0; //temp coeff of pressure offset C4
unsigned int T_REF= 0; //reference temp C5
unsigned int TEMPSENS= 0; // temp coeff of the temp C6

int Pressure;
int Temperature;

void InitU2(void) {
    U2BRG = 415; // PIC24FJ128GA010 data sheet, 17.1 for calculation, Fcy = 16MHz.
    //16MHz/9600/4-1 = 415
    U2MODE = 0x8008; // See data sheet, pg 148. Enable UART2, BRGH = 1,
    // Idle state = 1, 8 data, No parity, 1 Stop bit
    U2STA = 0x0400; // See data sheet, pg. 150, Transmit Enable
    // Following lines pertain Hardware handshaking
    //TRISFbits.TRISF13 = 1; // enable RTS , output
    //RTS = 1; // default status , not ready to send
}

void SPI1Init(void) {
    //0120
    SPI1CON1 = 0x0320; // Master Mode, 8-bit bytes, Idle state low, Active Low
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

void us_delay(int us) {
    T2CON = 0x8010; // Timer 2 on, TCKPS<1,0> = 01 thus 8 Prescale thus 2MHz
    TMR2 = 0;
    while (TMR2 < us * 2); // 1/16MHz/(8*2)) = 1us.)
}

void ms_delay(int ms) {
    T2CON = 0x8030; // Timer 2 on, TCKPS<1,0> = 11 thus 256 Prescale
    TMR2 = 0;
    while (TMR2 < ms * 63); // 1/16MHz/(256*63)) = 0.001008 close to 1 ms.)
}

unsigned int read_16bit_PROM(unsigned int address){
    unsigned int x = 0;
    PORTA = 0x00;
    writeSPI1(address); //send 8-bit command sequence
    
    unsigned int xHIGH = readSPI1(); //read 8 MSBs
    unsigned int xLOW = readSPI1();
    PORTA = 0x01;
    
    x = (xLOW | (xHIGH << 8));
    
    return x;
}

unsigned long read_conversion_data(unsigned int address){
    unsigned long x = 0;
    PORTA = 0x00;
    writeSPI1(address); //send 8-bit command sequence
    PORTA = 0x01;
    
    ms_delay(2);
    PORTA = 0x00;
    writeSPI1(0x00);
    unsigned long xHIGH = readSPI1(); //read 8 MSBs
    unsigned long xMID = readSPI1();
    unsigned long xLOW = readSPI1();
    PORTA = 0x01;
    
    x = (xLOW | (xMID << 8) | (xHIGH << 16));
    
    return x;
}

void barSetup(void){
//read calibration data from PROM
SENS_T1 = read_16bit_PROM(0b10100010); //send 8-bit command sequence, read C1
OFF_T1 = read_16bit_PROM(0b10100100); //send 8-bit command sequence, read C2
TCS = read_16bit_PROM(0b10100110); //send 8-bit command sequence, read C3
TCO = read_16bit_PROM(0b10101000); //send 8-bit command sequence, read C4
T_REF = read_16bit_PROM(0b10101010); //send 8-bit command sequence, read C5
TEMPSENS = read_16bit_PROM(0b10101100); //send 8-bit command sequence, read C6
}

void getPressureAndTemp()
{
    //Read digital pressure and temp data
    unsigned long D1 = read_conversion_data(0x42); //digital pressure value  
    unsigned long D2 = read_conversion_data(0x52); //digital temp value
    
    //Calculate temperature
    long dT = D2 - (T_REF *pow(2,8));
       
    int TEMP = 2000 + (dT*TEMPSENS/pow(2,23)); //actual temperature
    
    //Calculate temperature compensated pressure
    long long OFF = OFF_T1*pow(2,17) + (TCO*dT)/pow(2,6);
    long long SENS = SENS_T1*pow(2,16)+(TCS*dT)/pow(2,7);
    
    //account for non-linearity of low temp
    if (TEMP < 2000)
    {
        long T2 = pow(dT,2)/pow(2, 31);
        long long OFF2 = 61*pow((TEMP-2000),2)/pow(2,4);
        long long SENS2 = 2*pow((TEMP-2000),2);
        
        TEMP = TEMP -T2;
        OFF = OFF -OFF2;
        SENS = SENS-SENS2;
    }
    
    long P = ((D1 *SENS)/pow(2,21)-OFF)/pow(2,15); //pressure
    
    double d_temp = TEMP/100.0;
    double d_pressure = P/100.0;
    
    printf("Temp (degrees C): %.2f\n", d_temp);
    printf("Pressure: (mbar): %.2f\n", d_pressure);
}

void main(void) {
    __C30_UART = 2;
    TRISA = 0x00;
    PORTA = 0x01; //RA0, PIN 17
    
    SPI1Init(); // Initialize SPI port.
    InitU2();
    
    PORTA = 0x00;
    ms_delay(10);
    
    barSetup(); // Setup bar. pressure
    
    PORTA = 0x01;
    
    while (1) {
        ms_delay(500);
        getPressureAndTemp();
    }
}