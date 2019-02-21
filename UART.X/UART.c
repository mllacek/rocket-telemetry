//Senior Design UART communication with PC

#include "config.h"
#include <time.h>
#include <stdlib.h>

char getU1(void);
char putU2(char);
void ms_delay(int);

void _ISR _U1RXInterrupt (void)
{
    if (IEC0bits.U1RXIE && IFS0bits.U1RXIF){
        char c = getU1();
        putU2(c);
    }
    
    IFS0bits.U1RXIF = 0 ;   //  clear  interrupt  flag

}

void us_delay(int useconds) {
    T2CON = 0x8010; //8 to 1 prescaler
    TMR2 = 0; //Clear timer
    while (TMR2 < useconds * 2) {
        //Waste clock cycle
    }
}

void ms_delay(int ms) {
    T2CON = 0x8030; // Timer 2 on, TCKPS<1,0> = 11 thus 256 Prescale
    TMR2 = 0;
    while (TMR2 < ms * 63); // 1/16MHz/(256*63)) = 0.001008 close to 1 ms.)
}

void InitU2(void) {
    U2BRG = 415; // PIC24FJ128GA010 data sheet, 17.1 for calculation, Fcy = 16MHz.
    //16MHz/9600/4-1 = 415
    U2MODE = 0x8008; // See data sheet, pg 148. Enable UART2, BRGH = 1,
    // Idle state = 1, 8 data, No parity, 1 Stop bit
    U2STA = 0x0400; // See data sheet, pg. 150, Transmit Enable
    // Following lines pertain Hardware handshaking
    //TRISFbits.TRISF13 = 1; // enable RTS , output
    //RTS = 1; // default status , not ready to send
}

void InitU1(void) {
    U1BRG = 415; // PIC24FJ128GA010 data sheet, 17.1 for calculation, Fcy = 16MHz.
    //16MHz/9600/4-1 = 415
    
    //Enable UART Interrupts
    IPC2bits.U1RXIP = 1; //Set Uart RX Interrupt Priority (interrupt on reception allowed)
     
    U1MODE = 0x8008; // See data sheet, pg 148. Enable UART1, BRGH = 1,
    // Idle state = 1, 8 data, No parity, 1 Stop bit
    U1STA = 0x0400; // See data sheet, pg. 150, Transmit Enable
    // Following lines pertain Hardware handshaking
    //TRISFbits.TRISF13 = 1; // enable RTS , output
    //RTS = 1; // default status , not ready to send
    U1STAbits.URXISEL=00; //interrupt when a character is received
    IEC0bits.U1RXIE = 1; //Enable Receive Interrupt

    //Putting the UART interrupt flag down.
    IFS0bits.U1RXIF = 0;
}
char putU1(char c) { // (Transmit)
    //while (CTS); //wait for !CTS (active low)
    while (U1STAbits.UTXBF); // Wait if transmit buffer full.
    U1TXREG = c; // Write value to transmit FIFO
    return c;
}

char putU2(char c) { // (Transmit)
    //while (CTS); //wait for !CTS (active low)
    while (U2STAbits.UTXBF); // Wait if transmit buffer full.
    U2TXREG = c; // Write value to transmit FIFO
    return c;
}

char getU1 ( void ) //(Receive)
{
//RTS = 0; // telling the other side !RTS
//while (! U1STAbits . URXDA ); // wait
//RTS =1; // telling the other side RTS
//if (U1STAbits.OERR){
//    putU2("t");
//}
    char c = U1RXREG;
return c ; // from receiving buffer
} //getU2 (get one byte)

char getU2 ( void ) //(Receive)
{
//RTS = 0; // telling the other side !RTS
while (! U2STAbits . URXDA ); // wait
//RTS =1; // telling the other side RTS
return U2RXREG ; // from receiving buffer
} //getU2

void U2_send_string(char* st_pt)
{
    while(*st_pt) //if there is a char
        putU2(*st_pt++);//process it as a byte data
}


void main(void) {
    InitU1();
    InitU2();
    ms_delay(500);
    //srand(time(NULL));   // Initialization, should only be called once.
    
    //char buffer [50];
    //char c;
    //int t = 0;
    while (1) {
        //int r = rand()% 20; // Returns a pseudo-random integer between 0 and 19.
        //t = t + r;
        //sprintf (buffer, "%d", t);
        //U2_send_string(buffer);
        //ms_delay(2); // wait 500 us as suggested
        //putU2('\n');
        //ms_delay(2); // wait 500 us as suggested
        //r = rand()% 20;
        //t = t - r;
        //sprintf (buffer, "%d", t);
        //U2_send_string(buffer);
        //ms_delay(2); // wait 500 us as suggested
        //putU2('\n');
        //ms_delay(500);
        U2_send_string("Start");
        ms_delay(500);
        U2_send_string("Test");
        ms_delay(500);
    }
    
}