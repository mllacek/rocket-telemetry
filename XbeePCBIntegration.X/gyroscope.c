#include "gyroscope.h"
#include "spiConfig.h"

#include <stdio.h>
#include <xc.h>

void SPIGyroStart() {
    PORTA = 0x00;
}

void SPIGyroEnd() {
    PORTA = 0x01; //RA0, PIN 17
}

void gyroscopeInit(void){
    //TRISA = 0x00;
    //SPIBarEnd(); 

    SPIGyroStart();
    writeSPI1(L3GD20_REGISTER_CTRL_REG1);
    writeSPI1(0x0F);
    SPIGyroEnd();
    SPIGyroStart();
    writeSPI1(L3GD20_REGISTER_CTRL_REG4); //L3DS20_RANGE_2000DPS
    writeSPI1(0x20);
    SPIGyroEnd();
}

void gyroscopeDetect(void){
    int device_ID;
    SPIGyroStart();
    writeSPI1(L3GD20_REGISTER_WHO_AM_I | 0x80); //Reading From This Register
    device_ID = readSPI1();
    SPIGyroEnd();
    printf("Device ID = %x\n", device_ID);
    if (device_ID != 0xD7) {
        printf("The L3GD20 Gyroscope is NOT Detected...\n");
    } else {
        printf("L3GD20 Gyroscope Successfully Detected...\n");
    }
}

void printGyroscopeData(char* message){
    int xlo,xhi,ylo,yhi,zlo,zhi;
    int x,y,z;
    
    SPIGyroStart();
    writeSPI1(L3GD20_REGISTER_OUT_X_L | 0x80 | 0x40); //Reading Multiple Times From This Register
    us_delay(10); //MAY NEED TO ADJUST, ADD A DELAY 
    xlo = readSPI1();
    xhi = readSPI1();
    ylo = readSPI1();
    yhi = readSPI1();
    zlo = readSPI1();
    zhi = readSPI1();
    SPIGyroEnd();

    // Shift values to create properly formed integer (low byte first)
    x = (xlo | (xhi << 8));
    y = (ylo | (yhi << 8));
    z = (zlo | (zhi << 8));

    // Compensate Values Depending On Resolution
    x *= L3GD20_SENSITIVITY_2000DPS;
    y *= L3GD20_SENSITIVITY_2000DPS;
    z *= L3GD20_SENSITIVITY_2000DPS;
        
    //printf("%d,%d,%d,", x, y, z);
    sprintf(message, "%d,%d,%d,", x, y, z);
}
