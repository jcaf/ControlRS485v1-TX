/*
 * pinGetLevel.h
 *
 *  Created on: Dec 3, 2020
 *      Author: jcaf
 */

#ifndef SRC_PINGETLEVEL_PINGETLEVEL_H_
#define SRC_PINGETLEVEL_PINGETLEVEL_H_

struct _pinGetLevel
{
  	int8_t counterDebounce;
    struct _pinGetLevel_bf
    {
		unsigned sm0:1;
		unsigned level:1;
		unsigned level_last:1;
		unsigned changed:1;
		unsigned __a:4;
  	}bf;
};
void pinGetLevel_setChangedFlag(struct _pinGetLevel *pgl, int8_t i, int8_t level);
void pinGetLevel_init(struct _pinGetLevel *pgl, PTRFX_retINT8_T *pfx, int8_t PINGETLEVEL_NUMMAX, int8_t START_WITH_CHANGED_FLAG);
void pinGetLevel_job(struct _pinGetLevel *pgl, PTRFX_retINT8_T *pfx, int8_t PINGETLEVEL_NUMMAX);
enum _PGL_START_W_CHANGED_FLAG
{
	PGL_START_WITH_CHANGED_FLAG_OFF,
	PGL_START_WITH_CHANGED_FLAG_ON
};


#endif /* SRC_PINGETLEVEL_PINGETLEVEL_H_ */
