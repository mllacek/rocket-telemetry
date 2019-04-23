/*  GYROSCOPE CODE FOR SENIOR DESIGN
    Author: David Dalvin 
    Created on 03/21/2019
    Version 2.0
    GYROSCOPE Code
 */

#include <xc.h>
#include"spiConfiguration.h"
#include <stdio.h>
#include <libpic30.h>

#define MaxMsgSize 100

int message[MaxMsgSize];
int MsgLength = 0;


void InitU2(void) {
    U2BRG = 415; // PIC24FJ128GA010 data sheet, 17.1 for calculation, Fcy = 16MHz.
    U2MODE = 0x8008; // See data sheet, pg 148. Enable UART2, BRGH = 1,
    // Idle state = 1, 8 data, No parity, 1 Stop bit
    U2STA = 0x0400; // See data sheet, pg. 150, Transmit Enable
    // Following lines pertain Hardware handshaking
    //TRISFbits.TRISF13 = 1; // enable RTS , output
    //RTS = 1; // default status , not ready to send
}

void SPIStart() {
    PORTBbits.RB2 = 0; // CS Pin Pin 23 //RB2
}

void SPIEnd() {
    PORTBbits.RB2 = 1; // CS Pin Pin 23 //RB2
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
    writeSPI1(frame.StartDelim); //Start
    writeSPI1(frame.LengthMSB); //Length MSB, Equals total number of bytes in Frame data fields
    writeSPI1(frame.LengthLSB); //Length LSB

    //Frame data fields
    writeSPI1(frame.API_Id); //API Identifier, Transmit request frame
    writeSPI1(frame.Frame_ID); //cmdData, Frame ID
    writeSPI1(frame.Dest_64bit_1); //cmdData, begin 64 bit destination, currently set to broadcast
    writeSPI1(frame.Dest_64bit_2); //cmdData
    writeSPI1(frame.Dest_64bit_3); //cmdData
    writeSPI1(frame.Dest_64bit_4); //cmdData
    writeSPI1(frame.Dest_64bit_5); //cmdData
    writeSPI1(frame.Dest_64bit_6); //cmdData
    writeSPI1(frame.Dest_64bit_7); //cmdData
    writeSPI1(frame.Dest_64bit_8); //cmdData end 64 bit destination
    writeSPI1(frame.Reserved_1); //cmdData, Reserved
    writeSPI1(frame.Reserved_2); //cmdData, Reserved
    writeSPI1(frame.BrdCst_Rad); //cmdData, Broadcast radius
    writeSPI1(frame.TxOptions); //cmdData, Transmit options, P2MP
    //Start message here
    int i = 0;
    for(i = 0; i < frame.LengthLSB - 14; i++)
    {
        writeSPI1(frame.MsgBytes[i]);
    }
    //End message here
    //End Frame data fields
    writeSPI1(frame.Checksum); //Checksum
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
//    tx_frame = BuildFrame(tx_frame);
//    tx_frame = evalCheckSum(tx_frame);

    InitU2();
    __C30_UART = 2;

    printf("Starting Code...\n");
    ms_delay(500); //CAN CHANGE LATER TO LOWER VALUE
    ms_delay(500); //REMOVE LATER

    SPI1Init();
    printf("Initializing SPI...\n");
    ms_delay(500);
    ms_delay(500);
    AD1PCFG = 0x04; // Pin RB2 in digital mode
    TRISBbits.TRISB2 = 0; //Setting TRIS Bit to Digital Output

    SetMsg("Hi David! It's me!",18);
    
    while (1) {
        tx_frame = BuildFrame(tx_frame);
        TxSend(tx_frame);
        ms_delay(500);
        ms_delay(500);
        SetMsg("Groupon",7);
        printf("Packet Sent\n");
    }

    return 0;
}
