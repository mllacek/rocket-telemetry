//Senior Design UART communication with PC

#include "config.h"
#include <time.h>
#include <stdlib.h>

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
    U1MODE = 0x8008; // See data sheet, pg 148. Enable UART1, BRGH = 1,
    // Idle state = 1, 8 data, No parity, 1 Stop bit
    U1STA = 0x0400; // See data sheet, pg. 150, Transmit Enable
    // Following lines pertain Hardware handshaking
    //TRISFbits.TRISF13 = 1; // enable RTS , output
    //RTS = 1; // default status , not ready to send
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
while (! U1STAbits . URXDA ); // wait
//RTS =1; // telling the other side RTS
return U1RXREG ; // from receiving buffer
} //getU2

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
    char c;
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
        
        c = getU1();
        ms_delay(500);
        putU2(c);
        ms_delay(500);
        U2_send_string("Test");
        ms_delay(500);
    }
    
}