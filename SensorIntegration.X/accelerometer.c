#include "accelerometer.h"
#include "spiConfig.h"

#include <stdio.h>
#include <xc.h>

void SPIAccStart() {
    PORTBbits.RB2 = 0; // CS Pin Pin 23 //RB2

}

void SPIAccEnd() {
    PORTBbits.RB2 = 1; // CS Pin Pin 23 //RB2

}

void accelerometerInit(void){
    //AD1PCFG = 0x04; // Pin RB2 in digital mode
    //TRISBbits.TRISB2 = 0; //Setting TRIS Bit to Digital Output
    SPIAccStart();
    writeSPI1(LIS3DH_REG_CTRL1);
    writeSPI1(0xF7);
    SPIAccEnd();
    SPIAccStart();
    writeSPI1(LIS3DH_REG_CTRL4);
    writeSPI1(0x98);
    SPIAccEnd();
}

void accelerometerDetect(void){
    int device_ID;
    SPIAccStart();
    writeSPI1(LIS3DH_REG_WHOAMI | 0x80); //Reading From This Register
    device_ID = readSPI1();
    SPIAccEnd();
    printf("Device ID = %x\n", device_ID);
    if (device_ID != 0x33) {
        printf("The LIS3DH Accelerometer is NOT Detected...\n");
    } else {
        printf("LIS3DH Accelerometer Successfully Detected...\n");
    }
}

void printAccelerometerData(char* message) {
    int x,y,z;
    SPIAccStart();
    writeSPI1(LIS3DH_REG_OUT_X_L | 0x80 | 0x40); //Reading Multiple Times From This Register
    x = readSPI1();
    x |= readSPI1() << 8;
    y = readSPI1();
    y |= readSPI1() << 8;
    z = readSPI1();
    z |= readSPI1() << 8;
    SPIAccEnd();
    //printf("%d,%d,%d,", x, y, z);
    sprintf(message, "%d,%d,%d,", x, y, z);  
}

