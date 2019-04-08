#include "config.h"
#include "uartConfig.h"

//The GPS uses UART1

void _ISR _U1RXInterrupt (void)
{
    if (IEC0bits.U1RXIE && IFS0bits.U1RXIF){
        char c = getU1();
        putU2(c); 
    }
    
    //TODO: needs to be a printf
    
    IFS0bits.U1RXIF = 0 ;   //  clear  interrupt  flag

}

void InitU1(void) {
    U1BRG = 415; // PIC24FJ128GA010 data sheet, 17.1 for calculation, Fcy = 16MHz.
    //16MHz/9600/4-1 = 415
    
    //Enable UART Interrupts
    IPC2bits.U1RXIP = 1; //Set Uart RX Interrupt Priority (interrupt on reception allowed)
     
    U1MODE = 0x8008; // See data sheet, pg 148. Enable UART1, BRGH = 1,
    // Idle state = 1, 8 data, No parity, 1 Stop bit
    U1STA = 0x0400; // See data sheet, pg. 150, Transmit Enable

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
