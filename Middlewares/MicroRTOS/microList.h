/*
 *  Author: Honrun
 */
#ifndef _microList_H_
#define _microList_H_

#include "stdint.h"


#define MR_LIST_VALUE_MAX 0x7FFFFFFFFFFFFFFFLL
#define MR_LIST_VALUE_MIN 0x8000000000000000LL


typedef struct MrListStruct{
    struct MrListStruct *pPrev;
    struct MrListStruct *pNext;

    int64_t value;                  /* 链表节点排序值 */

    void *pElement;                 /* 当前节点所属 */
}MrListType;


int8_t cMrListInit(MrListType *ptypeBase, int64_t lValue);
int8_t cMrListInsert(MrListType *ptypeHead, MrListType *ptypeNew);
int8_t cMrListInsertAfter(MrListType *ptypeBase, MrListType *ptypeNew);
int8_t cMrListInsertBefore(MrListType *ptypeBase, MrListType *ptypeNew);
int8_t cMrListInsertHead(MrListType *ptypeHead, MrListType *ptypeNew);
int8_t cMrListInsertEnd(MrListType *ptypeHead, MrListType *ptypeNew);
int8_t cMrListRemove(MrListType *ptypeNow);


#endif // _microList_H_
