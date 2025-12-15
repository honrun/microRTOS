#ifndef _microTime_H_
#define _microTime_H_

#include "stdint.h"


int64_t lMrTickGet(void);
void vMrTickGather(void);
int64_t lMrTimeMonotonicGet(void);


#endif // _microTime_H_
