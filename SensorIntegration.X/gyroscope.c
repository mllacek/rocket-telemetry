#include "gyroscope.h"
#include "spiConfig.h"

#include <stdio.h>
#include <xc.h>

void SPIGyroStart() {
    PORTBbits.RB2 = 0; // CS Pin Pin 23 //RB2
}

void SPIGyroEnd() {
    PORTBbits.RB2 = 1; // CS Pin Pin 23 //RB2
}

void gyroscopeInit(void){
    AD1PCFG = 0x04; // Pin RB2 in digital mode
    TRISBbits.TRISB2 = 0; //Setting TRIS Bit to Digital Output

    SPIGyroStart();
    writeSPI2(L3GD20_REGISTER_CTRL_REG1);
    writeSPI2(0x0F);
    SPIGyroEnd();
    SPIGyroStart();
    writeSPI2(L3GD20_REGISTER_CTRL_REG4); //L3DS20_RANGE_2000DPS
    writeSPI2(0x20);
    SPIGyroEnd();
}

void gyroscopeDetect(void){
    int device_ID;
    SPIGyroStart();
    writeSPI2(L3GD20_REGISTER_WHO_AM_I | 0x80); //Reading From This Register
    device_ID = readSPI1();
    SPIGyroEnd();
    printf("Device ID = %x\n", device_ID);
    if (device_ID != 0xD7) {
        printf("The L3GD20 Gyroscope is NOT Detected...\n");
    } else {
        printf("L3GD20 Gyroscope Successfully Detected...\n");
    }
}

void printGyroscopeData(void){
    int xlo,xhi,ylo,yhi,zlo,zhi;
    int x,y,z;
    
    SPIGyroStart();
    writeSPI2(L3GD20_REGISTER_OUT_X_L | 0x80 | 0x40); //Reading Multiple Times From This Register
    us_delay(10); //MAY NEED TO ADJUST, ADD A DELAY 
    xlo = readSPI2();
    xhi = readSPI2();
    ylo = readSPI2();
    yhi = readSPI2();
    zlo = readSPI2();
    zhi = readSPI2();
    SPIGyroEnd();

    // Shift values to create properly formed integer (low byte first)
    x = (xlo | (xhi << 8));
    y = (ylo | (yhi << 8));
    z = (zlo | (zhi << 8));

    // Compensate Values Depending On Resolution
    x *= L3GD20_SENSITIVITY_2000DPS;
    y *= L3GD20_SENSITIVITY_2000DPS;
    z *= L3GD20_SENSITIVITY_2000DPS;
        
    printf("X=%d, Y=%d, Z=%d\n", x, y, z);
}
