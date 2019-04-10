/* 
 * File:   uartConfig.h
 * Author: user
 *
 * Created on April 8, 2019, 5:26 PM
 */

#ifndef UARTCONFIG_H
#define	UARTCONFIG_H
void InitU1(void);
char putU1(char c);
char getU1 ( void );
void InitU2(void);
char putU2(char c);
void U1_send_string(char* st_pt);
#endif	/* UARTCONFIG_H */

