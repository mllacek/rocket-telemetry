#include "spiConfig.h"
#include "uartConfig.h"
#include "gyroscope.h"
#include "accelerometer.h"
#include "barometric_pressure.h"

#include <libpic30.h>
#include <stdio.h>

int main(void) {
   
    InitU2();
    SPI1Init();
    SPI2Init();
    __C30_UART = 2;
    
    accelerometerInit();
    accelerometerDetect();
    
    barSetup();
    
    gyroscopeInit();
    gyroscopeDetect();
    
    while(1){
        ms_delay(100);
        printGyroscopeData();
        printPressureAndTemp();
        printAccelerometerData();
    }

    return 0;
}