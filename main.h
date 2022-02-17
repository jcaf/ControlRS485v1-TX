/*
 * main.h
 *
 *  Created on: Feb 5, 2022
 *      Author: jcaf
 */

#ifndef MAIN_H_
#define MAIN_H_


struct _mainflag
{
	unsigned sysTickMs :1;
	unsigned __a:7;
};
extern struct _mainflag mainflag;

#define SYSTICK_MS 10//10ms


//////////////////////////////
#define PORTWxBOTON_A 		PORTC
#define PORTRxBOTON_A 		PINC
#define CONFIGIOxBOTON_A 	DDRC
#define PINxBOTON_A		1


#define PORTWxBOTON_B 		PORTC
#define PORTRxBOTON_B 		PINC
#define CONFIGIOxBOTON_B 	DDRC
#define PINxBOTON_B		2

#define PORTWxBOTON_C 		PORTC
#define PORTRxBOTON_C 		PINC
#define CONFIGIOxBOTON_C 	DDRC
#define PINxBOTON_C		3

#define PORTWxBOTON_D 		PORTC
#define PORTRxBOTON_D 		PINC
#define CONFIGIOxBOTON_D 	DDRC
#define PINxBOTON_D		4
//////////////////////////////

#define PORTWxRS485_DIR 	PORTD
#define PORTRxRS485_DIR 	PIND
#define CONFIGIOxRS485_DIR 	DDRD
#define PINxRS485_DIR		2

#define PORTWxBUZZER 	PORTC
#define PORTRxBUZZER 	PINC
#define CONFIGIOxBUZZER DDRC
#define PINxBUZZER		5

#endif /* MAIN_H_ */
