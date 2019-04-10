#include "spiConfig.h"
#include "uartConfig.h"
#include "gyroscope.h"
#include "accelerometer.h"
#include "barometric_pressure.h"
#include "config.h"

#include <libpic30.h>
#include <stdio.h>

int main(void) {
   
    InitU2();
    InitU1();
    SPI1Init();
    SPI2Init();
    __C30_UART = 2;
    
    AD1PCFG = 0xFF; // Pins to digital
     
    TRISBbits.TRISB2 = 0; //Setting TRIS Bit to Digital Output
    PORTBbits.RB2 = 1; // CS Pin Pin 23 //RB2
    
    TRISGbits.TRISG9 = 0; //Setting TRIS Bit to Digital Output
    PORTGbits.RG9 = 1; // CS Pin Pin 14 //RG9
    
    TRISA = 0x00;
    PORTA = 0x01; //RA0, PIN 17
    
    ms_delay(2);
    barSetup();
    ms_delay(2);
    accelerometerInit();
    ms_delay(2);
    accelerometerDetect();
    
    gyroscopeInit();
    gyroscopeDetect();

    while(1){
        ms_delay(1000);
        printGyroscopeData();
        ms_delay(10);
        printPressureAndTemp();
        ms_delay(2);
        printAccelerometerData();
    }

    return 0;
}