/*  ACCELEROMETER CODE FOR SENIOR DESIGN
    Author: David Dalvin 
    Created on 03/21/2019
    Version 2.0
    ACCELEROMETER Code
 */
#include <xc.h>
#include"spiConfiguration.h"
#include <stdio.h>
#include <libpic30.h>

#define LIS3DH_REG_STATUS1       0x07
#define LIS3DH_REG_OUTADC1_L     0x08
#define LIS3DH_REG_OUTADC1_H     0x09
#define LIS3DH_REG_OUTADC2_L     0x0A
#define LIS3DH_REG_OUTADC2_H     0x0B
#define LIS3DH_REG_OUTADC3_L     0x0C
#define LIS3DH_REG_OUTADC3_H     0x0D
#define LIS3DH_REG_INTCOUNT      0x0E
#define LIS3DH_REG_WHOAMI        0x0F
#define LIS3DH_REG_TEMPCFG       0x1F
#define LIS3DH_REG_CTRL1         0x20
#define LIS3DH_REG_CTRL2         0x21
#define LIS3DH_REG_CTRL3         0x22
#define LIS3DH_REG_CTRL4         0x23
#define LIS3DH_REG_CTRL5         0x24
#define LIS3DH_REG_CTRL6         0x25
#define LIS3DH_REG_REFERENCE     0x26
#define LIS3DH_REG_STATUS2       0x27
#define LIS3DH_REG_OUT_X_L       0x28
#define LIS3DH_REG_OUT_X_H       0x29
#define LIS3DH_REG_OUT_Y_L       0x2A
#define LIS3DH_REG_OUT_Y_H       0x2B
#define LIS3DH_REG_OUT_Z_L       0x2C
#define LIS3DH_REG_OUT_Z_H       0x2D
#define LIS3DH_REG_FIFOCTRL      0x2E
#define LIS3DH_REG_FIFOSRC       0x2F
#define LIS3DH_REG_INT1CFG       0x30
#define LIS3DH_REG_INT1SRC       0x31
#define LIS3DH_REG_INT1THS       0x32
#define LIS3DH_REG_INT1DUR       0x33
#define LIS3DH_REG_CLICKCFG      0x38
#define LIS3DH_REG_CLICKSRC      0x39
#define LIS3DH_REG_CLICKTHS      0x3A
#define LIS3DH_REG_TIMELIMIT     0x3B
#define LIS3DH_REG_TIMELATENCY   0x3C
#define LIS3DH_REG_TIMEWINDOW    0x3D
#define LIS3DH_REG_ACTTHS        0x3E
#define LIS3DH_REG_ACTDUR        0x3F


//    PORTFbits.RF6 = // SPI1 Serial Clock Output Pin 55 //RF6 //Plugged Into Serial Clock of Accelerometer
//    PORTFbits.RF7 = // SPI1 Serial Data Input Pin 54 //RF7 //Plugged Into Serial Data Out of Accelerometer
//    PORTFbits.RF8 = // SPI1 Serial Data Output Pin 53 //RF8 //Plugged Into Serial Data In of Accelerometer

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

int device_ID;
int x;
int y;
int z;

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
    SPIStart();
    writeSPI1(LIS3DH_REG_CTRL1);
    writeSPI1(0xF7);
    SPIEnd();
    SPIStart();
    writeSPI1(LIS3DH_REG_CTRL4);
    writeSPI1(0x98);
    SPIEnd();


    SPIStart();
    writeSPI1(LIS3DH_REG_WHOAMI | 0x80); //Reading From This Register
    device_ID = readSPI1();
    SPIEnd();
    printf("Device ID = %x\n", device_ID);
    if (device_ID != 0x33) {
        printf("The LIS3DH Accelerometer is NOT Detected...\n");
        ms_delay(500);
        ms_delay(500);
    } else {
        printf("LIS3DH Accelerometer Successfully Detected...\n");
        ms_delay(500);
        ms_delay(500);
    }


    while (1) {
        SPIStart();
        writeSPI1(LIS3DH_REG_OUT_X_L | 0x80 | 0x40); //Reading Multiple Times From This Register
        x = readSPI1();
        x |= readSPI1() << 8;
        y = readSPI1();
        y |= readSPI1() << 8;
        z = readSPI1();
        z |= readSPI1() << 8;
        SPIEnd();
        ms_delay(100);
        printf("X=%d, Y=%d, Z=%d\n", x, y, z);
    }
    return 0;

}

