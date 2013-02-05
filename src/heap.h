#include "types.h"

/* 
 * Intial heap size to store 2000 orders
 * Heap will contain pointer to struct so 4 bytes*INIT_HEAP_SIZE
 * will not much. Heap size will keep on doubling after the heap is full
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
int cmp(Order *x, Order *y);
