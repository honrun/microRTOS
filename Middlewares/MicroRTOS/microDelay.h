#ifndef _microDelay_H_
#define _microDelay_H_

#include "stdint.h"


/* 延时链表 */
extern MrListType g_typeMrListDelay;


void vMrDelayTick(int64_t *plTickLast, int64_t lTick);
void vMrDelayMs(int64_t *plTickLast, int64_t lTime);
void vMrDelayCheck(void);


#endif // _microDelay_H_
