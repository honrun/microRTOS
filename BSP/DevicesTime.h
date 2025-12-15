/*
* Author: honrun
*/
#ifndef _DevicesTime_H_
#define _DevicesTime_H_

#include <stdint.h>


typedef struct{
    float UTC;
    int32_t year;
    uint8_t month  : 4;
    uint8_t day    : 5;
    uint8_t hour   : 5;
    uint8_t minute : 6;
    uint8_t second : 6;
    uint8_t week   : 3;
}TimeType;


uint8_t cTimeToWeek(int32_t iYear, int8_t cMonth, int8_t cDay);

void vStampToTime(TimeType *ptypeTime, int64_t lStamp, float fUTC);
int64_t lTimeToStamp(TimeType *ptypeTime);

int64_t lTimebaseGet(void);

void vTimestampSet(int64_t lUNIXTimestamp);
int64_t lTimestampGet(void);

void vTimeUTCSet(float fUTC);
float fTimeUTCGet(void);

char *pcStampToTimeStrings(int64_t lUNIXTimestamp);


#endif
