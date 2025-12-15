/*
 *  modifier: Honrun
 *  date:     2023/10/29 20:42
 */
#include "stdio.h"
#include "microConfig.h"
#include "microMemory.h"


typedef struct{
    size_t head;
    size_t tail;
}MrMemoryType;

#define MEM_REG(addr)  (*(volatile size_t *)((size_t)(addr)))


/* 动态内存头部信息 */
MrMemoryType g_typeMrMemoryHead = {0};
/* 需要增加 sizeof(size_t) 个字节，以使字节对齐 */
static uint8_t st_MrMemory[configMr_MEMORY_SIZE + sizeof(size_t)] = {0};


void vMrMemoryInit(void)
{
    MrMemoryType *ptypeMem = &g_typeMrMemoryHead;

    /* 地址 N 字节对齐 */
    ptypeMem->head = mrMemoryRoundUp((size_t)st_MrMemory);
    ptypeMem->tail = ptypeMem->head + configMr_MEMORY_SIZE;

    /* 初始化第一个空闲空间 */
    MEM_REG(ptypeMem->head) = ptypeMem->tail;
}

void *pvMrMemoryMalloc(size_t size)
{
    void *pvMem = NULL;
    MrMemoryType *ptypeMem = &g_typeMrMemoryHead;
    size_t head, free, headMin = 0;
    size_t sizeNow, sizeMin = (size_t)-1;

    if(size < 1)
        return NULL;

    /* 进入临界段 */
    vMrDisableIsr();

    /* 判断内存是否已经初始化 */
    if(ptypeMem->tail == 0)
        vMrMemoryInit();

    /* 长度 N 字节对齐 */
    size = mrMemoryRoundUp(size);

    /* 遍历mem空间 */
    for(head = ptypeMem->head; head < ptypeMem->tail; head = MEM_REG(head) & (~(size_t)1))
    {
        /* bit0 表示 当前块是否已被使用 */
        if((MEM_REG(head) & 1) == 0)
        {
            sizeNow = MEM_REG(head) - head - sizeof(size_t);

            /* 最小空闲匹配 */
            if((sizeNow >= size) && (sizeNow < sizeMin))
            {
                sizeMin = sizeNow;
                headMin = head;

                if(sizeMin == size)
                    break;
            }
        }
    }

    if(headMin != 0)
    {
        /* 最小匹配空间被划分后还有剩余空间，并能够存储下一个头部信息 */
        if((sizeMin - size) >= sizeof(size_t))
        {
            free = MEM_REG(headMin);
            MEM_REG(headMin) = headMin + sizeof(size_t) + size;

            /* 剩余的空闲空间 */
            MEM_REG(MEM_REG(headMin)) = free;
        }

        /* 返回空间管理信息之后的空间地址 */
        pvMem = (void *)(headMin + sizeof(size_t));

        /* 起始地址最低置1，表示已使用 */
        MEM_REG(headMin) |= 1;
    }

    /* 退出临界段 */
    vMrEnableIsr();

    return pvMem;
}

void *pvMrMemoryCalloc(size_t nitems, size_t size)
{
    void *pvMem;
    size_t *puiData;

    /* 长度 N 字节对齐 */
    size = mrMemoryRoundUp(nitems * size);

    if((pvMem = pvMrMemoryMalloc(size)) != NULL)
    {
        for(puiData = pvMem; size > 0; size -= sizeof(size_t))
        {
            *puiData++ = 0;
        }
    }

    return pvMem;
}

void *pvMrMemoryRealloc(void *pvMem, size_t size)
{
    void *pvHandle;
    MrMemoryType *ptypeMem = &g_typeMrMemoryHead;
    size_t *pTarget, *pSource;
    size_t head, tail, free;

    if((pvMem == NULL) || (size < 1))
        return NULL;

    /* 位移到头部管理信息位置 */
    head = (size_t)pvMem - sizeof(size_t);
    if((head < ptypeMem->head) || (ptypeMem->tail <= head))
        return NULL;

    /* 长度 N 字节对齐 */
    size = mrMemoryRoundUp(size);

    /* 进入临界段 */
    vMrDisableIsr();

    /* 计算后续衔接空闲空间的大小 */
    tail = MEM_REG(head) & (~(size_t)1);
    tail = (((MEM_REG(tail) & 1) == 0) && (tail < ptypeMem->tail)) ? MEM_REG(tail) : tail;

    free = tail - head - sizeof(size_t);

    /* 直接拓展衔接的空闲空间 */
    if(free >= size)
    {
        /* 多余的空间能够存储一个头部信息 */
        if((free - size) >= sizeof(size_t))
        {
            /* 截断成多余的空闲空间 */
            free = head + sizeof(size_t) + size;
            MEM_REG(free) = tail;

            tail = free;
        }

        MEM_REG(head) = tail | 1;

        pvHandle = pvMem;
    }
    /* 重新分配空间 */
    else if((pvHandle = pvMrMemoryMalloc(size)) != NULL)
    {
        /* 转移拷贝数据 */
        for(pTarget = (size_t *)pvHandle, pSource = (size_t *)pvMem; free > 0; free -= sizeof(size_t))
        {
            *pTarget++ = *pSource++;
        }

        /* 释放旧空间 */
        vMrMemoryFree(pvMem);
    }

    /* 退出临界段 */
    vMrEnableIsr();

    return pvHandle;
}

void vMrMemoryFree(void *pvMem)
{
    MrMemoryType *ptypeMem = &g_typeMrMemoryHead;
    size_t head, next;

    if(pvMem == NULL)
        return;

    /* 位移到头部管理信息位置 */
    head = (size_t)pvMem - sizeof(size_t);
    if((head < ptypeMem->head) || (ptypeMem->tail <= head))
        return;

    /* 进入临界段 */
    vMrDisableIsr();

    if((MEM_REG(head) & 1) != 0)
    {
        /* 最低位清零，表示释放此片内存 */
        MEM_REG(head) &= ~(size_t)1;

        /* 遍历，找到当前待删除空间的前面一个空间 */
        for(next = ptypeMem->head; MEM_REG(next) < head; next = MEM_REG(next) & (~(size_t)1));

        /* 合并前面一个相连的空闲空间 */
        if((MEM_REG(next) & 1) == 0)
        {
            MEM_REG(next) = MEM_REG(head);
            head = next;
        }

        /* 合并后面一个相连的空闲空间 */
        next = MEM_REG(head);
        if(((MEM_REG(next) & 1) == 0) && (next < ptypeMem->tail))
        {
            MEM_REG(head) = MEM_REG(next);
        }
    }

    /* 退出临界段 */
    vMrEnableIsr();
}
