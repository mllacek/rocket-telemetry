#include "accelerometer.h"
#include "spiConfig.h"

#include <stdio.h>
#include <xc.h>

void SPIStart() {
    PORTBbits.RB2 = 0; // CS Pin Pin 23 //RB2

}

void SPIEnd() {
    PORTBbits.RB2 = 1; // CS Pin Pin 23 //RB2

}

void accelerometerInit(void){
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
}

void accelerometerDetect(void){
    int device_ID;
    SPIStart();
    writeSPI1(LIS3DH_REG_WHOAMI | 0x80); //Reading From This Register
    device_ID = readSPI1();
    SPIEnd();
    printf("Device ID = %x\n", device_ID);
    if (device_ID != 0x33) {
        printf("The LIS3DH Accelerometer is NOT Detected...\n");
    } else {
        printf("LIS3DH Accelerometer Successfully Detected...\n");
    }
}

void printAccelerometerData(void) {
    int x,y,z;
    SPIStart();
    writeSPI1(LIS3DH_REG_OUT_X_L | 0x80 | 0x40); //Reading Multiple Times From This Register
    x = readSPI1();
    x |= readSPI1() << 8;
    y = readSPI1();
    y |= readSPI1() << 8;
    z = readSPI1();
    z |= readSPI1() << 8;
    SPIEnd();
    printf("X=%d, Y=%d, Z=%d\n", x, y, z);
}

