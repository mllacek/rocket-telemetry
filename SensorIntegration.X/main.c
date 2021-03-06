#include "spiConfig.h"
#include "uartConfig.h"
#include "gyroscope.h"
#include "accelerometer.h"
#include "barometric_pressure.h"
#include "config.h"
#include <string.h>

#include <libpic30.h>
#include <stdio.h>

int mSec = 0, Sec = 0, Min = 0; // Global variables

void _ISR _T1Interrupt(void) {

    mSec++; // increment the thousands of a second counter
    if (mSec > 999) {
        mSec = 0;
        Sec++; // increment the seconds counter
        if (Sec > 59) // 60 seconds make a minute
        {
            Sec = 0;
            Min++;
            if (Min > 180){
                Min = 0;
            }
        }
    } //seconds
    _T1IF = 0;
} // T1Interrupt

void TimerSetup(){
    _T1IP = 4; // this is the default value anyway
    TMR1 = 0; // clear the timer
    PR1 = 16000 - 1; // set the period register
    T1CON = 0x8000; // enabled, prescaler 1:64, internal clock
    _T1IF = 0; // clear the interrupt flag, before
    _T1IE = 1; // enable the T1 interrupt source
}

void append(char* s, char c)
{
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

void printTime(char* message){
    //printf("%d:%d.%d,", Min,Sec, mSec);
    sprintf(message, "$%d:%d.%d,", Min, Sec, mSec);  
}
       
int main(void) {
   
    TimerSetup();
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
    //accelerometerDetect();
    
    gyroscopeInit();
    //gyroscopeDetect();

    while(1){
        //ms_delay(50);
        
        char time[20];
        char gyro[20];
        char pt[20];
        char accel[20];
        char gps[50];
        
        printTime(time);
        printGyroscopeData(gyro);
        printPressureAndTemp(pt);
        printAccelerometerData(accel);
        printGpsData(gps);
        
        char mssg[500];
        sprintf(mssg, "");
        sprintf(mssg + strlen(mssg), time);
        sprintf(mssg + strlen(mssg), gyro);
        sprintf(mssg + strlen(mssg), pt);
        sprintf(mssg + strlen(mssg), accel);
        sprintf(mssg + strlen(mssg), gps);
        sprintf(mssg + strlen(mssg), "\n");
        
        printf(mssg);
        //TODO: ready to transmit
    }

    return 0;
}