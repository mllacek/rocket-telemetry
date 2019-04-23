#ifndef USW_FIFIO_H
#define	USW_FIFIO_H

#include <stdint.h>

#define FIFO_BUFFER_SIZE 512 // software buffer size (in bytes)

// UART data transmit function
//  - checks if there's room in the transmit sw buffer
//  - if there's room, it transfers data byte to sw buffer 
//  - automatically handles "uart_tx_buffer_full_flag"
//  - sets the overflow flag upon software buffer overflow (doesn't overwrite existing data)
//  - if this is the first data byte in the buffer, it enables the "hw buffer empty" interrupt
void uart_send_byte(uint8_t);


// UART data receive function
//  - checks if data exists in the receive sw buffer
//  - if data exists, it returns the oldest element contained in the buffer 
//  - automatically handles "uart_rx_buffer_full_flag"
//  - if no data exists, it clears the uart_rx_flag
uint8_t uart_get_byte(void);

void UART_rx_subroutine(char);

volatile extern uint8_t uart_rx_fifo_not_empty_flag; // this flag is automatically set and cleared by the software buffer
volatile extern uint8_t uart_rx_fifo_full_flag;      // this flag is automatically set and cleared by the software buffer
volatile extern uint8_t uart_rx_fifo_ovf_flag;       // this flag is not automatically cleared by the software buffer
volatile extern uint8_t uart_tx_fifo_full_flag;      // this flag is automatically set and cleared by the software buffer
volatile extern uint8_t uart_tx_fifo_ovf_flag;       // this flag is not automatically cleared by the software buffer
volatile extern uint8_t uart_tx_fifo_not_empty_flag; // this flag is automatically set and cleared by the software buffer

#endif