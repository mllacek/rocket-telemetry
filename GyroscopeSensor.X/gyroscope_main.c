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


#define L3GD20_ADDRESS                (0x6B)        // 1101011
#define L3GD20_POLL_TIMEOUT           (100)         // Maximum number of read attempts
#define L3GD20_ID                     0xD4
#define L3GD20H_ID                    0xD7

#define L3GD20_SENSITIVITY_250DPS  (0.00875F)      // Roughly 22/256 for fixed point match
#define L3GD20_SENSITIVITY_500DPS  (0.0175F)       // Roughly 45/256
#define L3GD20_SENSITIVITY_2000DPS (0.070F)        // Roughly 18/256
#define L3GD20_DPS_TO_RADS         (0.017453293F)  // degrees/s to rad/s multiplier


#define L3GD20_REGISTER_WHO_AM_I            0x0F   // 11010100   r
#define L3GD20_REGISTER_CTRL_REG1           0x20   // 00000111   rw
#define L3GD20_REGISTER_CTRL_REG2           0x21   // 00000000   rw
#define L3GD20_REGISTER_CTRL_REG3           0x22   // 00000000   rw
#define L3GD20_REGISTER_CTRL_REG4           0x23   // 00000000   rw
#define L3GD20_REGISTER_CTRL_REG5           0x24   // 00000000   rw
#define L3GD20_REGISTER_REFERENCE           0x25   // 00000000   rw
#define L3GD20_REGISTER_OUT_TEMP            0x26   //            r
#define L3GD20_REGISTER_STATUS_REG          0x27   //            r
#define L3GD20_REGISTER_OUT_X_L             0x28   //            r
#define L3GD20_REGISTER_OUT_X_H             0x29   //            r
#define L3GD20_REGISTER_OUT_Y_L             0x2A   //            r
#define L3GD20_REGISTER_OUT_Y_H             0x2B   //            r
#define L3GD20_REGISTER_OUT_Z_L             0x2C   //            r
#define L3GD20_REGISTER_OUT_Z_H             0x2D   //            r
#define L3GD20_REGISTER_FIFO_CTRL_REG       0x2E   // 00000000   rw
#define L3GD20_REGISTER_FIFO_SRC_REG        0x2F   //            r
#define L3GD20_REGISTER_INT1_CFG            0x30   // 00000000   rw
#define L3GD20_REGISTER_INT1_SRC            0x31   //            r
#define L3GD20_REGISTER_TSH_XH              0x32   // 00000000   rw
#define L3GD20_REGISTER_TSH_XL              0x33   // 00000000   rw
#define L3GD20_REGISTER_TSH_YH              0x34   // 00000000   rw
#define L3GD20_REGISTER_TSH_YL              0x35   // 00000000   rw
#define L3GD20_REGISTER_TSH_ZH              0x36   // 00000000   rw
#define L3GD20_REGISTER_TSH_ZL              0x37   // 00000000   rw
#define L3GD20_REGISTER_INT1_DURATION       0x38    // 00000000   rw

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
int xlo;
int xhi;
int ylo;
int yhi;
int zlo;
int zhi;
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
    writeSPI1(L3GD20_REGISTER_CTRL_REG1);
    writeSPI1(0x0F);
    SPIEnd();
    SPIStart();
    writeSPI1(L3GD20_REGISTER_CTRL_REG4); //L3DS20_RANGE_2000DPS
    writeSPI1(0x20);
    SPIEnd();


    SPIStart();
    writeSPI1(L3GD20_REGISTER_WHO_AM_I | 0x80); //Reading From This Register
    device_ID = readSPI1();
    SPIEnd();
    printf("Device ID = %x\n", device_ID);
    if (device_ID != 0xD7) {
        printf("The L3GD20 Gyroscope is NOT Detected...\n");
        ms_delay(500);
        ms_delay(500);
       
    } else {
        printf("L3GD20 Gyroscope Successfully Detected...\n");
        ms_delay(500);
        ms_delay(500);
     
    }

    while (1) {
        SPIStart();
        writeSPI1(L3GD20_REGISTER_OUT_X_L | 0x80 | 0x40); //Reading Multiple Times From This Register
        us_delay(10); //MAY NEED TO ADJUST, ADD A DELAY 
        xlo = readSPI1();
        xhi = readSPI1();
        ylo = readSPI1();
        yhi = readSPI1();
        zlo = readSPI1();
        zhi = readSPI1();
        SPIEnd();

        // Shift values to create properly formed integer (low byte first)
        x = (xlo | (xhi << 8));
        y = (ylo | (yhi << 8));
        z = (zlo | (zhi << 8));

        // Compensate Values Depending On Resolution
        x *= L3GD20_SENSITIVITY_2000DPS;
        y *= L3GD20_SENSITIVITY_2000DPS;
        z *= L3GD20_SENSITIVITY_2000DPS;
        
        ms_delay(10);
        printf("X=%d, Y=%d, Z=%d\n", x, y, z);
    }
    return 0;
}
