#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "uartConfig.h"
#include <stdbool.h>

#include <string.h>

uint8_t s_buffer[256];           // sentence buffer to save GPRMC data
volatile uint8_t cntr = 0;       // buffer index

volatile uint8_t gprmc = 0;      // flag to indicate GPRMC sentence
uint8_t rdy = 0;                 // flag to indicate if the s_buffer is ready to be processed

uint8_t msg_id[6];      // buffer to save msg ID, which will be 5 bytes long
uint8_t id_cntr = 6;    // buffer index
    
//The GPS uses UART1

void _ISR _U1RXInterrupt (void)
{
    
    if (IEC0bits.U1RXIE && IFS0bits.U1RXIF){
        
        char _char = getU1();         // temp variable to read next byte
        if(gprmc) {                                  // collect sentence data
            if(_char == '\r') {                      // until '\r' is read
                cntr = 0;                            // then reset buffer index
                gprmc = 0;
                rdy = 1;
                // and reset GPRMC indication
            } else {
                s_buffer[cntr++] = _char;            // store GPRMC sentence data
                //rdy = 1;                             // buffer ready to be processed
            }
        }

        if(id_cntr < 6) {                            // a '$' was received
            msg_id[id_cntr++] = _char;               // collect msg ID

            if(id_cntr == 5) {
                msg_id[id_cntr] = 0;                 // add terminating null

                if(strncmp(msg_id, "GPGGA", 5) == 0){// check for GPRMC
                    gprmc = 1;                       // indicate GPRMC sentence
                }
            }
        }

        if(_char == '$'){                            // wait for '$' to capture the msg ID
            id_cntr = 0;
        }

    }
    
    IFS0bits.U1RXIF = 0 ;   //  clear  interrupt  flag
}

void printGpsData(){
    printf("GPS:");
    if (rdy == 1){
            printf(s_buffer);
    }
    printf("\n");
}


void append(char* s, char c)
{
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

void InitU1(void) {
    U1BRG = 415; // PIC24FJ128GA010 data sheet, 17.1 for calculation, Fcy = 16MHz.
    //16MHz/9600/4-1 = 415
    
    //Enable UART Interrupts
    IPC2bits.U1RXIP = 1; //Set Uart RX Interrupt Priority (interrupt on reception allowed)
     
    U1MODE = 0x8008; // See data sheet, pg 148. Enable UART1, BRGH = 1,
    // Idle state = 1, 8 data, No parity, 1 Stop bit
    U1STA = 0x0400; // See data sheet, pg. 150, Transmit Enable
    
    U1_send_string("$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"); //GGA data only
    
    U1STAbits.URXISEL=00; //interrupt when a character is received
    IEC0bits.U1RXIE = 1; //Enable Receive Interrupt

    //Putting the UART interrupt flag down.
    IFS0bits.U1RXIF = 0;
}

char putU1(char c) { // (Transmit)
    while (U1STAbits.UTXBF); // Wait if transmit buffer full.
    U1TXREG = c; // Write value to transmit FIFO
    return c;
}

 char getU1 ( void ) //(Receive)
{
    char c = U1RXREG;
    return c ; // from receiving buffer
}

//UART2 is used for serial communication with the computer
void InitU2(void) {
    U2BRG = 415; // PIC24FJ128GA010 data sheet, 17.1 for calculation, Fcy = 16MHz.
    U2MODE = 0x8008; // See data sheet, pg 148. Enable UART2, BRGH = 1,
    // Idle state = 1, 8 data, No parity, 1 Stop bit
    U2STA = 0x0400; // See data sheet, pg. 150, Transmit Enable
}

char putU2(char c) { // (Transmit)
    //while (CTS); //wait for !CTS (active low)
    while (U2STAbits.UTXBF); // Wait if transmit buffer full.
    U2TXREG = c; // Write value to transmit FIFO
    return c;
}

void U1_send_string(char* st_pt)
{
    while(*st_pt) //if there is a char
        putU1(*st_pt++);//process it as a byte data
}
