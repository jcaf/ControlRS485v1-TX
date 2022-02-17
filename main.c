/*
 * ControlRS485v1-TX
 * main.c
 *
 *  Created on: Feb 5, 2022
 *      Author: jcaf
 *
 *      1) avrdude -c usbasp -B5 -p m328 -U lfuse:w:0xf7:m -U hfuse:w:0xd9:m -U efuse:w:0xfd:m
 *
 */


#include "main.h"

#include "pinGetLevel/pinGetLevel.h"
#include "serial/serial.h"
#include "usart/usart.h"
#include "adc/adc.h"
#include "indicator/indicator.h"

volatile struct _isr_flag
{
	unsigned sysTickMs :1;
	unsigned __a :7;
} isr_flag = { 0 };
struct _mainflag mainflag;


////////////////////////////////////////////////////////////////////

uint8_t checksum(char *str, uint8_t length);
char bin_to_asciihex(char c)//nibbleBin_to_asciiHex
{
    if (c < 10)
        return c+'0';
    else
        return (c-10) + 'A';
}

////////////////////////////////////////////////////////////////////

static
int8_t pinGetLevel_BOTON_A(void)
{
	return PinRead(PORTRxBOTON_A, PINxBOTON_A);
}
static
int8_t pinGetLevel_BOTON_B(void)
{
	return PinRead(PORTRxBOTON_B, PINxBOTON_B);
}
static
int8_t pinGetLevel_BOTON_C(void)
{
	return PinRead(PORTRxBOTON_C, PINxBOTON_C);
}
static
int8_t pinGetLevel_BOTON_D(void)
{
	return PinRead(PORTRxBOTON_D, PINxBOTON_D);
}

/////////////////////////////////////////////////////////
#define PINGETLEVEL_NUMMAX 4
enum _PINGETLEVEL_FX_INDEX
{
	PGLIDX_BOTON_A = 0,
	PGLIDX_BOTON_B,
	PGLIDX_BOTON_C,
	PGLIDX_BOTON_D
};

PTRFX_retINT8_T pinReadLevel[PINGETLEVEL_NUMMAX]=
{
		pinGetLevel_BOTON_A,
		pinGetLevel_BOTON_B,
		pinGetLevel_BOTON_C,
		pinGetLevel_BOTON_D
};
static struct _pinGetLevel pinGetLevel[PINGETLEVEL_NUMMAX];


/////////////////////////////////////////////////////////
int8_t stateButtonA;
int8_t stateButtonB;
int8_t stateButtonC;
int8_t stateButtonD;
int lock_BotonC = 0;
int lock_BotonD = 0;

/*
 *************************************************************/
uint8_t checksum(char *str, uint8_t length)
{
    uint16_t acc = 0;
    for (int i=0; i< length; i++)
    {
        acc += str[i];
    }
    return (uint8_t)(acc);
}
/**************************************************************
 * 	//Construir payload data + checksum
	//X = checksum
	//@RxxxxAxxxxBxxxxCxxxxDxxxxXcc'\r\n'
 *
 ***************************************************************/
uint16_t rv_light;
void tx_frame(void)
{
	char buff[10];
	char str[40];
	uint8_t checks;

	strcpy(str,"@R");//rv_light

	//
	itoa(rv_light, buff, 10);
	strcat(str,buff);

	//
	strcat(str,"A");
	itoa(pinGetLevel[PGLIDX_BOTON_A].bf.level, buff, 10);
	strcat(str,buff);
	//
	strcat(str,"B");
	itoa(pinGetLevel[PGLIDX_BOTON_B].bf.level, buff, 10);
	strcat(str,buff);
	//
	strcat(str,"C");
	//itoa(pinGetLevel[PGLIDX_BOTON_C].bf.level, buff, 10);
	itoa(stateButtonC, buff, 10);

	strcat(str,buff);

	strcat(str,"D");
	//itoa(pinGetLevel[PGLIDX_BOTON_D].bf.level, buff, 10);
	itoa(stateButtonD, buff, 10);
	strcat(str,buff);

	checks = checksum(str, strlen(str));

	strcat(str,"X");//Checksum

	//El checksum siempre sera de 2 caracteres, si es <10, entonces se rellena con "0"
	buff[1] = '\0';
	buff[0] = bin_to_asciihex(checks>>4);//primero la parte alta
	strcat(str,buff);
	buff[0] = bin_to_asciihex(checks & 0x0f);//primero la parte baja
	strcat(str,buff);
	//
	//strcat(str,"\r\n");
	strcat(str,"\r");//0x0D
	//
	usart_print_string(str);
}
int main(void)
{
	int8_t c = 0;

	PORTB=PORTC=PORTD = 0;

	PinTo1(PORTWxRS485_DIR, PINxRS485_DIR);//TX driver enable
	ConfigOutputPin(CONFIGIOxRS485_DIR, PINxRS485_DIR);

	//
	ConfigOutputPin(CONFIGIOxBUZZER, PINxBUZZER);
	indicator_setPortPin(&PORTWxBUZZER, PINxBUZZER);
	indicator_setKSysTickTime_ms(75/SYSTICK_MS);


	/* PinGetLevel section */
	PinTo1(PORTWxBOTON_A, PINxBOTON_A);//pullup
	ConfigInputPin(CONFIGIOxBOTON_A, PINxBOTON_A);

	PinTo1(PORTWxBOTON_B, PINxBOTON_B);//pullup
	ConfigInputPin(CONFIGIOxBOTON_B, PINxBOTON_B);

	PinTo1(PORTWxBOTON_C, PINxBOTON_C);//pullup
	ConfigInputPin(CONFIGIOxBOTON_C, PINxBOTON_C);

	PinTo1(PORTWxBOTON_D, PINxBOTON_D);//pullup
	ConfigInputPin(CONFIGIOxBOTON_D, PINxBOTON_D);

	__delay_ms(10);
	pinGetLevel_init(pinGetLevel, pinReadLevel, PINGETLEVEL_NUMMAX, PGL_START_WITH_CHANGED_FLAG_ON);

	/*********************/

	USART_Init ( (uint16_t) MYUBRR ); //9600 baud
	ADC_init(ADC_MODE_SINGLE_END);


	/*********************/
	//Config to 1ms
	TCNT0 = 0x00;
	TCCR0A = (1 << WGM01);
	TCCR0B =  (1 << CS02) | (0 << CS01) | (1 << CS00); 	//CTC, PRES=1024
	OCR0A = CTC_SET_OCR_BYTIME(10e-3, 1024);				//10ms PRES 1024 -> OCRA0 = 107
	//--> SYSTICK = 10e-3
	TIMSK0 |= (1 << OCIE0A);
	sei();
	//


	while (1)
	{
		if (isr_flag.sysTickMs)
		{
			isr_flag.sysTickMs = 0;
			mainflag.sysTickMs = 1;
		}
		//----------------------------------
		if (mainflag.sysTickMs)
		{
			if (++c >= (20/SYSTICK_MS) )
			{
				c = 0;
				pinGetLevel_job(pinGetLevel, pinReadLevel, PINGETLEVEL_NUMMAX);
				//

				//-------------------------------------------------
				if (pinGetLevel[PGLIDX_BOTON_A].bf.changed)
				{
					pinGetLevel[PGLIDX_BOTON_A].bf.changed = 0;

					if (pinGetLevel[PGLIDX_BOTON_A].bf.level == 0)
					{
						//val_boton_A = 1;
						indicator_setKSysTickTime_ms(75/SYSTICK_MS);
						indicator_On();
					}
				}
				//-------------------------------------------------
				if (pinGetLevel[PGLIDX_BOTON_B].bf.changed)
				{
					pinGetLevel[PGLIDX_BOTON_B].bf.changed = 0;

					if (pinGetLevel[PGLIDX_BOTON_B].bf.level == 0)
					{

						indicator_setKSysTickTime_ms(75/SYSTICK_MS);
						indicator_On();
					}
				}

				//-------------------------------------------------
				if (pinGetLevel[PGLIDX_BOTON_C].bf.changed)
				{
					pinGetLevel[PGLIDX_BOTON_C].bf.changed = 0;

					if (!lock_BotonC)

					{
						if (pinGetLevel[PGLIDX_BOTON_C].bf.level == 0)
						{
							lock_BotonD = 1;

							stateButtonC = 0; 	//Pressed

							//Buzzer
							indicator_setKSysTickTime_ms(75/SYSTICK_MS);
							indicator_On();
						}
						else
						{
							stateButtonC = 1;	//Release

							lock_BotonD = 0;
						}
					}

				}

				//-------------------------------------------------
				if (pinGetLevel[PGLIDX_BOTON_D].bf.changed)
				{
					pinGetLevel[PGLIDX_BOTON_D].bf.changed = 0;

					if (!lock_BotonD)

					{
						lock_BotonC = 1;

						if (pinGetLevel[PGLIDX_BOTON_D].bf.level == 0)
						{
							stateButtonD = 0;	//Pressed

							indicator_setKSysTickTime_ms(75/SYSTICK_MS);
							indicator_On();
						}
						else
						{
							stateButtonD = 1;	//Release

							lock_BotonC = 0;
						}
					}
				}
				//-------------------------------------------------
				rv_light = ADC_read(ADC_CH_0);
				tx_frame();

			}
		}

		indicator_job();

		//----------------------------------
		mainflag.sysTickMs = 0;
	}
}

//--> SYSTICK = 10e-3
ISR(TIMER0_COMPA_vect)
{
	isr_flag.sysTickMs = 1;
}

