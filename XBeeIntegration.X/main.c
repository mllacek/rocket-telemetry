#include "spiConfig.h"
#include "uartConfig.h"
#include "gyroscope.h"
#include "accelerometer.h"
#include "barometric_pressure.h"
#include "config.h"
#include <string.h>

#include <libpic30.h>
#include <stdio.h>

#define MaxMsgSize 84

int message[MaxMsgSize];
int MsgLength = 0;

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

void printTime(char* message){
    //printf("%d:%d.%d,", Min,Sec, mSec);
    sprintf(message, "$%d:%d.%d,", Min, Sec, mSec);  
}

void SPIStart() {
    PORTGbits.RG9 = 0;
}

void SPIEnd() {
    PORTGbits.RG9 = 1; 
}

struct TxFrame {
    int StartDelim; // Start Delimiter
    int LengthMSB; //Length MSB, Equals total number of bytes in Frame data fields
    int LengthLSB; //Length LSB
    int API_Id; //API Identifier, Transmit request frame
    int Frame_ID; //cmdData, Frame ID
    int Dest_64bit_1; //cmdData, begin 64 bit destination, currently set to broadcast
    int Dest_64bit_2;
    int Dest_64bit_3;
    int Dest_64bit_4;
    int Dest_64bit_5;
    int Dest_64bit_6;
    int Dest_64bit_7;
    int Dest_64bit_8; //cmdData end 64 bit destination
    int Reserved_1; //cmdData, Reserved
    int Reserved_2; //cmdData, Reserved
    int BrdCst_Rad; //cmdData, Broadcast radius
    int TxOptions; //cmdData, Transmit options, P2MP
    //Start message here
    int MsgBytes[MaxMsgSize];
    //End message here
    int Checksum; //Checksum, Add API_Id through MessByte_n, subtract least significant 8 bits from 0xFF
};

void SetMsgLength(int length)
{
    if((length <= MaxMsgSize)||(length >= 0))
    {
        MsgLength = length;
    }
}

void TxSend(struct TxFrame frame) {
    SPIStart();
    writeSPI2(frame.StartDelim); //Start
    writeSPI2(frame.LengthMSB); //Length MSB, Equals total number of bytes in Frame data fields
    writeSPI2(frame.LengthLSB); //Length LSB

    //Frame data fields
    writeSPI2(frame.API_Id); //API Identifier, Transmit request frame
    writeSPI2(frame.Frame_ID); //cmdData, Frame ID
    writeSPI2(frame.Dest_64bit_1); //cmdData, begin 64 bit destination, currently set to broadcast
    writeSPI2(frame.Dest_64bit_2); //cmdData
    writeSPI2(frame.Dest_64bit_3); //cmdData
    writeSPI2(frame.Dest_64bit_4); //cmdData
    writeSPI2(frame.Dest_64bit_5); //cmdData
    writeSPI2(frame.Dest_64bit_6); //cmdData
    writeSPI2(frame.Dest_64bit_7); //cmdData
    writeSPI2(frame.Dest_64bit_8); //cmdData end 64 bit destination
    writeSPI2(frame.Reserved_1); //cmdData, Reserved
    writeSPI2(frame.Reserved_2); //cmdData, Reserved
    writeSPI2(frame.BrdCst_Rad); //cmdData, Broadcast radius
    writeSPI2(frame.TxOptions); //cmdData, Transmit options, P2MP
    //Start message here
    int i = 0;
    for(i = 0; i < frame.LengthLSB - 14; i++)
    {
        writeSPI2(frame.MsgBytes[i]);
    }
    //End message here
    //End Frame data fields
    writeSPI2(frame.Checksum); //Checksum
    SPIEnd();
    SetMsgLength(0);
}

struct TxFrame BuildFrame(struct TxFrame frame) {
    frame.StartDelim = 0x7E; // Start Delimiter
    frame.LengthMSB = 0x00; //Length MSB, Equals total number of bytes in Frame data fields
    frame.LengthLSB = MsgLength + 14; //Length LSB
    frame.API_Id = 0x10; //API Identifier, Transmit request frame
    frame.Frame_ID = 0x01; //cmdData, Frame ID
    frame.Dest_64bit_1 = 0x00; //cmdData, begin 64 bit destination, currently set to broadcast
    frame.Dest_64bit_2 = 0x13;
    frame.Dest_64bit_3 = 0xA2;
    frame.Dest_64bit_4 = 0x00;
    frame.Dest_64bit_5 = 0x41;
    frame.Dest_64bit_6 = 0x90;
    frame.Dest_64bit_7 = 0x91;
    frame.Dest_64bit_8 = 0x5D; //cmdData end 64 bit destination
    frame.Reserved_1 = 0xFF; //cmdData, Reserved
    frame.Reserved_2 = 0xFE; //cmdData, Reserved
    frame.BrdCst_Rad = 0x00; //cmdData, Broadcast radius
    frame.TxOptions = 0x40; //cmdData, Transmit options, P2MP
    //Start message here
    int i = 0;
    for(i = 0; i < frame.LengthLSB - 14; i++)
    {
        frame.MsgBytes[i] = message[i];
    }
    //End message here
    frame.Checksum = evalCheckSum(frame); //Checksum, Add API_Id through MessByte_n, subtract least significant 8 bits from 0xFF

    return frame;
}

int evalCheckSum(struct TxFrame frame) {
    int sum = 0;
    int checksum = 0;
    int msgsum = 0;
    int i = 0;
    for(i = 0; i < frame.LengthLSB - 14; i++)
    {
        msgsum = msgsum + frame.MsgBytes[i];
    }
    sum = frame.API_Id + frame.Frame_ID + frame.Dest_64bit_1 + frame.Dest_64bit_2 
            + frame.Dest_64bit_3 + frame.Dest_64bit_4 + frame.Dest_64bit_5 + 
            frame.Dest_64bit_6 + frame.Dest_64bit_7 + frame.Dest_64bit_8 + 
            frame.Reserved_1 + frame.Reserved_2 + frame.BrdCst_Rad + frame.TxOptions
            + msgsum;
    sum = sum & 0xFF;
    checksum = 0xFF - sum;
    return checksum;
}

void SetMsg(char* msg, int msgLen)
{
    int i = 0;
    for(i = 0; i < msgLen; i++)
    {
        message[i] = msg[i];
    }
    SetMsgLength(msgLen); 
}
       
int main(void) {
    struct TxFrame tx_frame;
       
    TimerSetup();
    InitU2();
    InitU1();
    SPI1Init();
    SPI2Init();
    __C30_UART = 2;
    
    AD1PCFG = 0xFF; // Pins to digital
    
    //accel
    TRISBbits.TRISB2 = 0; //Setting TRIS Bit to Digital Output
    PORTBbits.RB2 = 1; // CS Pin Pin 23 //RB2
    
    //bar pressure
    TRISBbits.TRISB1 = 0; //Setting TRIS bit to digital output
    PORTBbits.RB1 = 1;
    
    //Xbee
    TRISGbits.TRISG9 = 0; //Setting TRIS Bit to Digital Output
    PORTGbits.RG9 = 1; // CS Pin Pin 14 //RG9
    
    //gyro
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
        printf("Length:%d", strlen(mssg));
        
        SetMsg(mssg, strlen(mssg));
        
        tx_frame = BuildFrame(tx_frame);
        TxSend(tx_frame);
        ms_delay(250);
        //ms_delay(500);
        //
        //printf("Packet Sent\n");
                
    }

    return 0;
}