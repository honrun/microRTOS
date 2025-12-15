/*
 *  modifier: Honrun
 *  date:     2023/10/29 20:42
 */
#include "stdio.h"
#include "microList.h"


int8_t cMrListInit(MrListType *ptypeBase, int64_t lValue)
{
    if(ptypeBase == NULL)
        return -1;

    ptypeBase->pNext = ptypeBase->pPrev = ptypeBase;
    ptypeBase->value = lValue;

    return 0;
}

/* 从小到大，顺序插入 */
int8_t cMrListInsert(MrListType *ptypeHead, MrListType *ptypeNew)
{
    MrListType *ptypeNow = NULL;

    if((ptypeHead == NULL) || (ptypeNew == NULL))
        return -1;

    /* 找到排序插入的前一个位置 */
    for(ptypeNow = ptypeHead; ptypeNew->value > ptypeNow->pNext->value; ptypeNow = ptypeNow->pNext)
    {
        /* 链表最后一个节点 */
        if(ptypeNow->pNext == ptypeHead)
        {
            break;
        }
    }

    return cMrListInsertAfter(ptypeNow, ptypeNew);
}

/* 插到之后 */
int8_t cMrListInsertAfter(MrListType *ptypeBase, MrListType *ptypeNew)
{
    if((ptypeBase == NULL) || (ptypeNew == NULL))
        return -1;

    ptypeNew->pNext         = ptypeBase->pNext;
    ptypeBase->pNext->pPrev = ptypeNew;
    ptypeBase->pNext        = ptypeNew;
    ptypeNew->pPrev         = ptypeBase;

    return 0;
}

/* 插到之前 */
int8_t cMrListInsertBefore(MrListType *ptypeBase, MrListType *ptypeNew)
{
    return cMrListInsertAfter(ptypeNew, ptypeBase);
}

/* 插到第一个位置 */
int8_t cMrListInsertHead(MrListType *ptypeHead, MrListType *ptypeNew)
{
    return cMrListInsertAfter(ptypeHead, ptypeNew);
}

/* 插到最后一个位置 */
int8_t cMrListInsertEnd(MrListType *ptypeHead, MrListType *ptypeNew)
{
    return cMrListInsertAfter(ptypeNew, ptypeHead);
}

/* 移除 */
int8_t cMrListRemove(MrListType *ptypeList)
{
    if(ptypeList == NULL)
        return -1;

    ptypeList->pPrev->pNext = ptypeList->pNext;
    ptypeList->pNext->pPrev = ptypeList->pPrev;

    ptypeList->pNext = ptypeList->pPrev = ptypeList;

    return 0;
}
