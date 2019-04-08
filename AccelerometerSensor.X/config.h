/* 
 * File:   config.h
 * Author: DDalv
 *
 * Created on January 22, 2019, 2:12 PM
 */

/*
 ** config.h
 ** 
 ** This header file will be used for all our projects.
 ** See PIC24FJ128GA010 data sheet, CH24 regarding Configuration.
 ** MACRO commands _CONFIG1 and _CONFIG2 are used.
 */
#ifndef INCLUDE_CONFIG_H
#define INCLUDE_CONFIG_H
#include <xc.h>          // Generic header file, contains all definitions 
// for our PIC24 processor (& others)
_CONFIG1( JTAGEN_OFF     // disable JTAG interface           0x3FFF 0x3 = 0b0011
        & GCP_OFF        // disable general code protection  0x7FFF 0x7 = 0b0111
        & GWRP_OFF       // disable flash write protection   0x7FFF 0x7 = 0b0111
        & ICS_PGx2       // ICSP interface (2=default)       0x7FFF 0x7 = 0b0111
        & FWDTEN_OFF)    // disable watchdog timer           0x7F7F 0x7 = 0b0111
        
_CONFIG2( IESO_OFF       // two speed start up disabled      0x7FFF 0x7 = 0b0111
        & FCKSM_CSDCMD   // disable clock-swithcing/ monitor 0xFFBF 0xB = 0b1011
        & FNOSC_PRIPLL   // primary oscillator: enable PLL   0xFBFF 0xB = 0b1011
        & POSCMOD_XT)    // primary oscillator: XT mode      0xFFFD 0xD = 0b1101
#endif