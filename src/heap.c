#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

/* 
 * Create a heap. INIT_HEAP_SIZE can be chosen to any number since
 * the memory allocation has doubling growth strategy
 */
heap_t *init_heap()
{
        heap_t *h = (heap_t *)malloc(sizeof(heap_t));
        h->heap = (Order **)malloc(INIT_HEAP_SIZE*sizeof(Order *));
        h->heap_size = 0;
        h->total_array_size = INIT_HEAP_SIZE;
        return h;
}

/* 
 * Double the memory allocated to the heap and move all the heap data 
 * to the new space if alloacted 
 */
void grow(heap_t *h)
{
        h->heap = (Order **)realloc(h->heap, 2*(h->total_array_size)*sizeof(Order *));
        h->total_array_size = 2*h->total_array_size;
}

/* 
 * Reduce the memory allocated to the heap and move all the heap data 
 * to the new space if allocated. Currently not using the shrink logic.
 * Might use in future for optimizing the memory
 */
void shrink(heap_t *h)
{
        int new_size = (h->total_array_size)/2;
        h->heap = (Order **)realloc(h->heap, new_size*sizeof(Order *));
        h->total_array_size = new_size;
}

/*
 * Insert an element into the heap. Takes O(log N)
 */
void insert(heap_t *h, Order **order)
{
        if(h->total_array_size < ((h->heap_size)+2))
                grow(h);

        h->heap[h->heap_size] = *order;
        int i = h->heap_size;
        while(i!=0 && (cmp(h->heap[PARENT(i)], h->heap[i])<0)){
                int p = PARENT(i);
                Order *temp = h->heap[i];
                h->heap[i] = h->heap[p];
                h->heap[p] = temp;
                i = p;
        }
        (h->heap_size)++;
}

/* 
 * Get the maximum element from the heap and also delete it.
 * Peeking on the max takes O(1) time however deleting it too
 * takes O(log N) time
 */
Order **extract_max(heap_t *h)
{
        if(h->heap_size == 0)
                return NULL;
        if(h->heap_size == 1) {
                h->heap_size = 0;
                return h->heap;
        }
        /* Swapping the last element */
        Order *temp = *((h->heap) + (h->heap_size) - 1);
        h->heap[h->heap_size-1] = *h->heap;
        *h->heap = temp;

        --(h->heap_size);
        heapify(h, 0);
        return (h->heap + h->heap_size);
}

/* 
 * Heapify the memory allocated. Implements a max heapify
 */
void heapify(heap_t *h, int i)
{
        if(h == NULL || h->heap == NULL){
                fprintf(stderr, "Heapify called on NULL heap");
        }
        Order **heap = h->heap;
        int l = LEFT(i);
        int r = RIGHT(i);
        int max = i;
        if (l >= (h->total_array_size)){
                grow(h);
        }
        if (l >= (h->heap_size))
                return;
        if(cmp(heap[l], heap[max]) > 0)
                max = l;
        if (r >= (h->heap_size))
                return;
        if(cmp(heap[r], heap[max]) > 0)
                max = r;
        if (max != i) {
                Order *temp = heap[i];
                heap[i] = heap[max];
                heap[max] = temp;
                heapify(h, max);
        }
}
