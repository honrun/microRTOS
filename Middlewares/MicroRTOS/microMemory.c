/*
 *  modifier: Honrun
 *  date:     2023/10/29 20:42
 */
#include "stdio.h"
#include "microConfig.h"
#include "microMemory.h"


typedef struct{
    void *next;
    void *tail;
}MrMemoryType;


/* 动态内存头部信息 */
MrMemoryType g_typeMrMemoryHead = {0};
/* 需要增加 sizeof(size_t) 个字节，以使字节对齐 */
static uint8_t st_MrMemory[configMr_MEMORY_SIZE + sizeof(size_t)] = {0};


void vMrMemoryInit(void)
{
    MrMemoryType *ptypeMem = &g_typeMrMemoryHead;

    /* 地址 N 字节对齐 */
    ptypeMem->next = (void *)mrMemoryRoundUp((size_t)st_MrMemory);
    ptypeMem->tail = ptypeMem->next + configMr_MEMORY_SIZE;

    /* 初始化第一个空闲空间 */
    ((MrMemoryType *)ptypeMem->next)->next = ptypeMem->tail;
}

void *pvMrMemoryMalloc(size_t size)
{
    void *pvMem = NULL;
    MrMemoryType *ptypeMem = &g_typeMrMemoryHead, *now, *free = NULL;
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
    for(now = ptypeMem->next; (void *)now < ptypeMem->tail; now = (void *)((size_t)now->next & (~(size_t)1)))
    {
        /* bit0 表示 当前块是否已被使用 */
        if(((size_t)now->next & 1) == 0)
        {
            sizeNow = (size_t)now->next - (size_t)now - sizeof(void *);

            /* 最小空闲匹配 */
            if((sizeNow >= size) && (sizeNow < sizeMin))
            {
                sizeMin = sizeNow;
                free = now;

                if(sizeMin == size)
                    break;
            }
        }
    }

    if(free != NULL)
    {
        /* 最小匹配空间被划分后还有剩余空间，并能够存储下一个头部信息 */
        if((sizeMin - size) >= sizeof(void *))
        {
            now = free->next;
            free->next = (void *)free + sizeof(void *) + size;

            /* 剩余的空闲空间 */
            ((MrMemoryType *)free->next)->next = now;
        }

        /* 起始地址最低置1，表示已使用 */
        free->next += 1;

        /* 返回空间管理信息之后的空间地址 */
        pvMem = (void *)free + sizeof(void *);
    }

    /* 退出临界段 */
    vMrEnableIsr();

    return pvMem;
}

void *pvMrMemoryCalloc(size_t nitems, size_t size)
{
    void *pvMem;
    size_t *pData;

    /* 长度 N 字节对齐 */
    size = mrMemoryRoundUp(nitems * size);

    if((pvMem = pvMrMemoryMalloc(size)) != NULL)
    {
        for(pData = pvMem; size > 0; size -= sizeof(size_t))
        {
            *pData++ = 0;
        }
    }

    return pvMem;
}

void *pvMrMemoryRealloc(void *pvMem, size_t size)
{
    void *pvHandle;
    MrMemoryType *ptypeMem = &g_typeMrMemoryHead, *now, *free, *next;
    size_t *pTarget, *pSource, sizeFree;

    if((pvMem == NULL) || (size < 1))
        return NULL;

    /* 位移到头部管理信息位置 */
    now = pvMem - sizeof(void *);
    if(((void *)now < ptypeMem->next) || (ptypeMem->tail <= (void *)now))
        return NULL;

    /* 长度 N 字节对齐 */
    size = mrMemoryRoundUp(size);

    /* 进入临界段 */
    vMrDisableIsr();

    /* 计算后续衔接空闲空间的大小 */
    next = now->next - 1;
    next = (((size_t)next->next & 1) == 0) && ((void *)next < ptypeMem->tail) ? next->next : next;

    sizeFree = (size_t)next - (size_t)now - sizeof(void *);

    /* 直接拓展衔接的空闲空间 */
    if(sizeFree >= size)
    {
        /* 多余的空间能够存储一个头部信息 */
        if((sizeFree - size) >= sizeof(void *))
        {
            /* 截断成多余的空闲空间 */
            free = (void *)now + sizeof(void *) + size;
            free->next = next;

            next = free;
        }

        now->next = (void *)next + 1;

        pvHandle = pvMem;
    }
    /* 重新分配空间 */
    else if((pvHandle = pvMrMemoryMalloc(size)) != NULL)
    {
        /* 转移拷贝数据 */
        for(pTarget = (size_t *)pvHandle, pSource = (size_t *)pvMem; sizeFree > 0; sizeFree -= sizeof(size_t))
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
    MrMemoryType *ptypeMem = &g_typeMrMemoryHead, *now, *free;

    if(pvMem == NULL)
        return;

    /* 位移到头部管理信息位置 */
    free = pvMem - sizeof(void *);
    if(((void *)free < ptypeMem->next) || (ptypeMem->tail <= (void *)free))
        return;

    /* 进入临界段 */
    vMrDisableIsr();

    if(((size_t)free->next & 1) != 0)
    {
        /* 最低位清零，表示释放此片内存 */
        free->next -= 1;

        /* 遍历，找到当前待删除空间的前面一个空间 */
        for(now = ptypeMem->next; now->next < (void *)free; now = (void *)((size_t)now->next & (~(size_t)1)));

        /* 合并前面一个相连的空闲空间 */
        if(((size_t)now->next & 1) == 0)
        {
            now->next = free->next;
            free = now;
        }

        /* 合并后面一个相连的空闲空间 */
        now = free->next;
        if((((size_t)now->next & 1) == 0) && ((void *)now < ptypeMem->tail))
        {
            free->next = now->next;
        }
    }

    /* 退出临界段 */
    vMrEnableIsr();
}
