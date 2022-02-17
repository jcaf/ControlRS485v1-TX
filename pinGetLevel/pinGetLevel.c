/*
 * pgl.c
 *
 *  Created on: Dec 3, 2020
 *      Author: jcaf
 */

#include <stdint.h>
#include "../main.h"
#include "pinGetLevel.h"

#define PINGETLEVEL_PERIODIC_ACCESS 20	//msE-3	aplication-level
#define PINGETLEVEL_SCAN_DEBOUNCE_MIN 20//ms 	added 2020

//#include <math.h>
//#define PINGETLEVEL_SCAN_KCOUNT_DEBOUNCE (int)( PINGETLEVEL_PERIODIC_ACCESS < PINGETLEVEL_SCAN_DEBOUNCE_MIN ? ceil((PINGETLEVEL_SCAN_DEBOUNCE_MIN*1.0f)/PINGETLEVEL_PERIODIC_ACCESS): 1)
#define PINGETLEVEL_SCAN_KCOUNT_DEBOUNCE (int)( PINGETLEVEL_PERIODIC_ACCESS < PINGETLEVEL_SCAN_DEBOUNCE_MIN ? CEIL_INTEGERS(PINGETLEVEL_SCAN_DEBOUNCE_MIN, PINGETLEVEL_PERIODIC_ACCESS): 1)


void pinGetLevel_setChangedFlag(struct _pinGetLevel *pgl, int8_t i, int8_t level)
{
	pgl[i].bf.changed = level;//provocar un cambio inicial
}

void pinGetLevel_init(struct _pinGetLevel *pgl, PTRFX_retINT8_T *pfx, int8_t PINGETLEVEL_NUMMAX, int8_t START_WITH_CHANGED_FLAG)
{
    //__delay_ms(1);								//dar un tiempo para que se setee los pull-ups
    for (int i=0; i<PINGETLEVEL_NUMMAX; i++)
    {
        pgl[i].bf.level = pfx[i]();
        pgl[i].bf.level_last = pgl[i].bf.level;
        //
        pinGetLevel_setChangedFlag(pgl, i, START_WITH_CHANGED_FLAG);
    }
}

void pinGetLevel_job(struct _pinGetLevel *pgl, PTRFX_retINT8_T *pfx, int8_t PINGETLEVEL_NUMMAX)
{
    int8_t level_temp;

    for (int8_t i = 0; i< PINGETLEVEL_NUMMAX; i++)
    {
        if  (pgl[i].bf.sm0 == 0)
        {
            level_temp = pfx[i]();

            if (pgl[i].bf.level_last != level_temp)
            {
                pgl[i].bf.level_last = level_temp;//save
                pgl[i].bf.sm0 = 1;
            }
        }
        else if (pgl[i].bf.sm0 == 1)
        {
            if (++pgl[i].counterDebounce >= PINGETLEVEL_SCAN_KCOUNT_DEBOUNCE)//ms
            {
                pgl[i].counterDebounce = 0x0;

                level_temp = pfx[i]();

                if (pgl[i].bf.level_last == level_temp)
                {
                    pgl[i].bf.level = level_temp;   //pgl[i].bf.level_last;}
                    pgl[i].bf.changed = 1;          //clear in app-level
                }
                else
                {
                	pgl[i].bf.level_last = level_temp;//Bug fixed
                }
                pgl[i].bf.sm0 = 0;
            }
        }
    }
}
