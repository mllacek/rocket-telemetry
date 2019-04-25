#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "uartConfig.h"
#include <stdbool.h>

#include <string.h>

uint8_t gps_buffer[256];           // sentence buffer to save GPRMC data
volatile uint8_t cntr = 0;       // buffer index
volatile uint8_t gpgga = 0;      // flag to indicate GPRMC sentence
uint8_t msg_id[6];      // buffer to save msg ID, which will be 5 bytes long
uint8_t id_cntr = 6;    // buffer index
    
const char delims[] = ",";

//The GPS uses UART1

void _ISR _U1RXInterrupt (void)
{
    if (IEC0bits.U1RXIE && IFS0bits.U1RXIF){
        //Adapted from : https://electronics.stackexchange.com/questions/258189/process-and-store-gps-data-on-mcu
        char _char = getU1();         // temp variable to read next byte
        if(gpgga) {                                  // collect sentence data
            if(_char == '\r') {                      // until '\r' is read
                gps_buffer[cntr] = '\0'; //add terminating null
                cntr = 0;                            // then reset buffer index
                gpgga = 0;
                // and reset GPGGA indication
            } else {
                gps_buffer[cntr++] = _char;            // store GPRMC sentence data
            }
        }

        if(id_cntr < 6) {                            // a '$' was received
            msg_id[id_cntr++] = _char;               // collect msg ID

            if(id_cntr == 5) {
                msg_id[id_cntr] = '\0';                 // add terminating null

                if(strncmp(msg_id, "GPGGA", 5) == 0){// check for GPRMC
                    gpgga = 1;                       // indicate GPRMC sentence
                }
            }
        }

        if(_char == '$'){                            // wait for '$' to capture the msg ID
            id_cntr = 0;
        }

    }
    
    IFS0bits.U1RXIF = 0 ;   //  clear  interrupt  flag
}

void printGpsData(char* message){
    const char *s = gps_buffer; //copy gps buffer
    int field_count = 0;
    sprintf(message, "");
    do {
        size_t field_len = strcspn(s, delims);
        if (field_count >= 2 && field_count < 5){ //fields 2 through 5
            //printf("%.*s,", (int)field_len, s);
            sprintf(message + strlen(message), "%.*s,", (int)field_len, s);
        }
        if (field_count == 5) {
            //printf("%.*s", (int)field_len, s);
            sprintf(message + strlen(message), "%.*s", (int)field_len, s);
        }
        //GPS altitude
        //if (field_count == 9)
         //   printf(",%.*s", (int)field_len, s);
        s += field_len;
        field_count++;
    } while (*s++);
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
