#ifndef _microMemory_H_
#define _microMemory_H_


/* 使x对n字节对齐 */
#define mrMemoryRoundUp(x)  (((x) + (sizeof(void *) - 1)) & (~(sizeof(void *) - 1)))


void *pvMrMemoryMalloc(size_t size);
void *pvMrMemoryCalloc(size_t nitems, size_t size);
void *pvMrMemoryRealloc(void *pvMem, size_t size);
void vMrMemoryFree(void *pvMem);


#endif // _microMemory_H_
