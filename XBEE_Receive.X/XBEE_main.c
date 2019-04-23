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

#define SPI_ATTn PORTEbits.RE9

#define MaxMsgSize 100
#define MaxRxBuffSize 127
int ReceiveBuffer[MaxRxBuffSize];
int message[MaxMsgSize];
int MsgLength = 0;
int RxLength = 0;

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

int checkHeader() { // timeout is in milliseconds
  int Length = 0;
  int inByte = 0;

  while (inByte != 0x7E) {
      inByte = readSPI1(); // read a byte from the buffer 
  }
  if (inByte == 0x7E) { // if we got the API start byte
    int lengthMSB = readSPI1(); // read the most significant length byte
    int lengthLSB = readSPI1(); // read the least significant length byte
    Length = (lengthMSB << 8) + lengthLSB; // put the two bytes together
  }
  return Length;

}

char getIdentifier() {

 char apiIdentifier = 'Z'; // set apiIdentifier to an impossible value
    apiIdentifier = (char)readSPI1();
    return apiIdentifier; // returns null if timed out

}

char* getLongAdress(char* _address){

   char addrByte = 'Z';
   int i = 0;
       for (i = 0; i<8; i++){

       addrByte =(char)readSPI1();

       _address[i]=addrByte;

       }

      return _address; //returns null if timed out
}

int getLocaladdress(){

 int localAdress =0;
    int localAd = readSPI1(); // read the most significant length byte

    int localAd2 = readSPI1(); // read the least significant length byte

    localAdress = (localAd << 8) + localAd2; // put the two bytes together

     return localAdress; // returns null if timed out

 }

 int getOptions(){

 int options =0x00;
     options = (int)readSPI1(); // get the options 1 packet ack returned 2 brodcast

     return options; // returns null if timed out
 }

void getInfo(int _packetLength){

  int addrByte = 0x00;
  MsgLength = _packetLength - 12;
    int i= 0;
      for (i= 0; i < MsgLength; i++) { // take out other info in packet + checksu

        message[i] = (int)readSPI1(); // get a byte of data
       }
}
 
 char getCheckSum(){

    char checksum ='Z';

    checksum = (char)readSPI1();

    return checksum; // returns null if timed out

 }

 void SendMessageOut()
 {
     int i= 0;
      for (i= 0; i < MsgLength; i++) { // take out other info in packet + checksum

        printf("%c, ",message[i]); // get a byte of data
       } 
 } 
 
void RxReceive() {
    SPIStart();
    MsgLength = 0;
    MsgLength = checkHeader();
    if (MsgLength > 0 ) { // if we have a packet continue

    int apiIdentifier = getIdentifier(); // GET ID

    char Adress[8];// array for 64 bit address

    char* adress = getLongAdress(Adress);//GET LONG ADRESS don't really need pointer

    int localAdress = getLocaladdress();// GET LOCAL ADDRESS

    int options = getOptions();//GET OPTION 

    getInfo(MsgLength);//GET INFO

    char checksum = getCheckSum(); //GET CHECKSUM

    SendMessageOut();
  }
    SPIEnd();
}


int main(void) {
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

    while (1) {
        if (SPI_ATTn == 0) { //Waiting for XBEE Module to have data to send
            printf("Packet Received\n");
            RxReceive();
            printf("\n");
            ms_delay(500);
            ms_delay(500);
        }
    }
    return 0;
}
