#ifndef __TYPES
#define __TYPES 
#include <stdbool.h>
#include "../lib/uthash.h"

#define MAX_NAME_LEN 20

typedef struct {
        char name[MAX_NAME_LEN];
        int price;
        int quantity;
        int timestamp;
        bool is_bid;
}Order;

struct node{
        int price;
        struct node *next;
};

typedef struct node offer_node_t, bid_node_t;

typedef struct {
        char name[MAX_NAME_LEN];
        int shares;
        UT_hash_handle hh;
}shares_t;

typedef struct {
        Order **heap;  /* Pointer to a list of orders */
        int heap_size; /* Number of elements in the heap */
        int total_array_size; /* Total memory allocated */
}heap_t;

typedef struct {
        int price;
        heap_t *h;
        UT_hash_handle hh;
}hash_t;

#endif
