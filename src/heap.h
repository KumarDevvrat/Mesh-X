#include "types.h"

/* 
 * Initial heap size to store 2000 orders
 * Heap will contain pointer to struct so 4 bytes*INIT_HEAP_SIZE
 * will not be much. Heap size will keep on doubling after the heap is full
 */
#define INIT_HEAP_SIZE 2000
#define LEFT(i) 2*i+1
#define RIGHT(i) 2*i+2
#define PARENT(i) (i-1)/2

heap_t *init_heap();
void grow(heap_t *h);
void insert(heap_t *h, Order **order);
Order **extract_max(heap_t *h);
void heapify(heap_t *h, int i);
static inline int cmp(Order *x, Order *y);

/*
 * Compare function between two orders. If the order is old then 
 * it gets the priority after the quantity priority is taken into 
 * account
 */
static inline int cmp(Order *x, Order *y)
{
        if(x->quantity > y->quantity)
                return 1;
        else if(x->quantity < y->quantity)
                return -1;

        if(x->timestamp > y->timestamp)
                return -1;
        else
                return 1;
}
