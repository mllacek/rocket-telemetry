#include "barometric_pressure.h"
#include "spiConfig.h"

#include <math.h>
#include <xc.h>
#include <stdio.h>

unsigned int SENS_T1 = 0; //pressure sensitivity C1
unsigned int OFF_T1= 0; //pressure offset, C2
unsigned int TCS= 0; //temp coefficient of pressure sensitivity C3
unsigned int TCO= 0; //temp coeff of pressure offset C4
unsigned int T_REF= 0; //reference temp C5
unsigned int TEMPSENS= 0; // temp coeff of the temp C6

//TODO
void SPIBarStart() {
    PORTBbits.RB2 = 0; // CS Pin Pin 23 //RB2

}

void SPIBarEnd() {
    PORTBbits.RB2 = 1; // CS Pin Pin 23 //RB2

}

unsigned int read_16bit_PROM(unsigned int address){
    unsigned int x = 0;
    PORTA = 0x00;
    writeSPI1(address); //send 8-bit command sequence
    
    unsigned int xHIGH = readSPI1(); //read 8 MSBs
    unsigned int xLOW = readSPI1();
    PORTA = 0x01;
    
    x = (xLOW | (xHIGH << 8));
    
    return x;
}

unsigned long read_conversion_data(unsigned int address){
    unsigned long x = 0;
    PORTA = 0x00;
    writeSPI1(address); //send 8-bit command sequence
    PORTA = 0x01;
    
    ms_delay(2);
    PORTA = 0x00;
    writeSPI1(0x00);
    unsigned long xHIGH = readSPI1(); //read 8 MSBs
    unsigned long xMID = readSPI1();
    unsigned long xLOW = readSPI1();
    PORTA = 0x01;
    
    x = (xLOW | (xMID << 8) | (xHIGH << 16));
    
    return x;
}

void barSetup(void){
    TRISA = 0x00;
    PORTA = 0x01; //RA0, PIN 17
    
    PORTA = 0x00;
    //read calibration data from PROM
    SENS_T1 = read_16bit_PROM(0b10100010); //send 8-bit command sequence, read C1
    OFF_T1 = read_16bit_PROM(0b10100100); //send 8-bit command sequence, read C2
    TCS = read_16bit_PROM(0b10100110); //send 8-bit command sequence, read C3
    TCO = read_16bit_PROM(0b10101000); //send 8-bit command sequence, read C4
    T_REF = read_16bit_PROM(0b10101010); //send 8-bit command sequence, read C5
    TEMPSENS = read_16bit_PROM(0b10101100); //send 8-bit command sequence, read C6
    PORTA = 0x01;
}

void printPressureAndTemp()
{
    //Read digital pressure and temp data
    unsigned long D1 = read_conversion_data(0x42); //digital pressure value  
    unsigned long D2 = read_conversion_data(0x52); //digital temp value
    
    //Calculate temperature
    long dT = D2 - (T_REF *pow(2,8));
       
    int TEMP = 2000 + (dT*TEMPSENS/pow(2,23)); //actual temperature
    
    //Calculate temperature compensated pressure
    long long OFF = OFF_T1*pow(2,17) + (TCO*dT)/pow(2,6);
    long long SENS = SENS_T1*pow(2,16)+(TCS*dT)/pow(2,7);
    
    //account for non-linearity of low temp
    if (TEMP < 2000)
    {
        long T2 = pow(dT,2)/pow(2, 31);
        long long OFF2 = 61*pow((TEMP-2000),2)/pow(2,4);
        long long SENS2 = 2*pow((TEMP-2000),2);
        
        TEMP = TEMP -T2;
        OFF = OFF -OFF2;
        SENS = SENS-SENS2;
    }
    
    long P = ((D1 *SENS)/pow(2,21)-OFF)/pow(2,15); //pressure
    
    double d_temp = TEMP/100.0;
    double d_pressure = P/100.0;
    
    printf("Temp (degrees C): %.2f\n", d_temp);
    printf("Pressure: (mbar): %.2f\n", d_pressure);
}


