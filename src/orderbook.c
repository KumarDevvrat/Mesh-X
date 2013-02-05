#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "orderbook.h"
#include "heap.h"

/* 
 * In the given file prices are limited to 0.01 to 100.00
 * Since prices are multiplied by 100 to have an integer Value, 
 * these seem to safer lower and upper bounds
 */
int higest_bid_price = -1;
int lowest_offer_price = 20000;

/* 
 * Hash table maintaing the price as the key and corresponding
 * heap(orders) as the Value
 */
hash_t *list_of_offers = NULL;
hash_t *list_of_bidders = NULL;

shares_t *trader_shares = NULL;

/* Singly linked list to keep track of next price */
offer_node_t *offer_list = NULL;
bid_node_t *bid_list = NULL;

int buy_shares(int offerprice, heap_t *h, int num_shares)
{
        Order *offer;
        while(h->heap_size != 0) {
                offer = *h->heap;
                extract_max(h);
                if(num_shares<offer->quantity){
                        /* If the top element has more shares then decrease it's Value
                         * and put it again in the heap */
                        offer->quantity = offer->quantity-num_shares;
                        insert(h, &offer);
                        update_trader_shares(offer->name, num_shares, false);
                        num_shares = 0;
                        break;
                } else {
                        update_trader_shares(offer->name, offer->quantity, false);
                        num_shares = num_shares - offer->quantity;
                        free(offer);
                }
        }

        /* If all the shares are bought from the heap then inform the 
         * data structures about it */
        if(h->heap_size == 0)
                remove_offers(offerprice);
        return num_shares;
}

int sell_shares(int bidprice, heap_t *h, int num_shares)
{
        Order *bid;
        while(h->heap_size != 0) {
                bid = *h->heap;
                extract_max(h);
                if(num_shares<(bid->quantity)){
                        bid->quantity = bid->quantity-num_shares;
                        /* If the top element has more shares then decrease it's Value
                         * and put it again in the heap */
                        insert(h, &bid);
                        update_trader_shares(bid->name, num_shares, true);
                        num_shares = 0;
                        break;
                } else {
                        update_trader_shares(bid->name, bid->quantity, true);
                        num_shares = num_shares - bid->quantity;
                        free(bid);
                }
        }

        /* If all the shares are sold from the heap then inform the 
         * data structures about it */
        if(h->heap_size == 0)
                remove_bids(bidprice);
        return num_shares;
}

void process_bid(Order **order)
{
        offer_node_t *lowest_offer = get_lowest_offer_price((*order)->price);
        if (lowest_offer == NULL) {
                /* No higher bid for this offer so add it in the unprocessed buy heap */
                add_bid(order);
                return;
        }

        int num_shares_to_be_bought = (*order)->quantity;
        int offerprice = lowest_offer->price;
        hash_t *offers = get_offers(offerprice);
        while(num_shares_to_be_bought>0 && offers->h->heap_size != 0) {
                num_shares_to_be_bought = buy_shares(offerprice, offers->h, num_shares_to_be_bought);
                lowest_offer = lowest_offer->next;
                if (lowest_offer->price > (*order)->price)
                        break;
                if (num_shares_to_be_bought>0 && lowest_offer->price != 20000){
                        offerprice = lowest_offer->price;
                        offers = get_offers(offerprice);
                } else {
                        break;
                }
        }

        update_trader_shares((*order)->name, (*order)->quantity-num_shares_to_be_bought, true);

        /* If not all shares were bought then add it to the unprocessed buy heap */
        if (num_shares_to_be_bought>0){
                (*order)->quantity = num_shares_to_be_bought;
                add_bid(order);
        }
}

void process_offer(Order **order)
{
        bid_node_t *highest_bid = get_highest_bidding_price((*order)->price);
        if(highest_bid == NULL){
                /* No higher bid for this offer so add it in the unprocessed sell heap */
                add_offer(order);
                return;
        }

        int num_shares_to_be_sold = (*order)->quantity;
        int bidprice = highest_bid->price;
        hash_t *bids = get_bidders(bidprice);
        while(num_shares_to_be_sold>0 && bids->h->heap_size != 0){
                num_shares_to_be_sold = sell_shares(bidprice, bids->h, num_shares_to_be_sold);
                highest_bid = highest_bid->next;
                if(highest_bid->price < (*order)->price)
                        break;
                if(num_shares_to_be_sold>0 && highest_bid->price != -1) {
                        bidprice = highest_bid->price;
                        bids = get_bidders(bidprice);
                } else {
                        break;
                }
        }

        update_trader_shares((*order)->name, (*order)->quantity-num_shares_to_be_sold, false);

        /* If not all shares were sold then add it to the unprocessed sell heap */
        if(num_shares_to_be_sold>0) {
                (*order)->quantity = num_shares_to_be_sold;
                add_offer(order);
        }
}

void init_orderbook()
{
        /* Create head and tail sentinal nodes here to avoid NULL checking
         * in various loops */
        bid_node_t *bid_list_head = (bid_node_t*)malloc(sizeof(bid_node_t));
        bid_node_t *bid_list_tail = (bid_node_t*)malloc(sizeof(bid_node_t));
        bid_list_head->price = 20000;
        bid_list_tail->price = -1;
        bid_list_head->next = bid_list_tail;
        bid_list = bid_list_head;

        offer_node_t *offer_list_head = (offer_node_t*)malloc(sizeof(offer_node_t));
        offer_node_t *offer_list_tail = (offer_node_t*)malloc(sizeof(offer_node_t));
        offer_list_head->price = -1;
        offer_list_tail->price = 20000;
        offer_list_head->next = offer_list_tail;
        offer_list = offer_list_head;

        higest_bid_price = -1;
        lowest_offer_price = 20000;

        list_of_offers = NULL;
        list_of_bidders = NULL;
}

/* 
 * Add an unprocessed bid. Create a heap if not already there and add it
 * to linked list as well
 */
void add_bid(Order **bid)
{
        Order *b = *bid;
        if (b->price > higest_bid_price)
                higest_bid_price = b->price;

        hash_t *ht;
        HASH_FIND_INT(list_of_bidders, &(b->price), ht);
        if(ht == NULL) {
                ht = (hash_t *)malloc(sizeof(hash_t));
                ht->price = b->price;
                ht->h = init_heap();
                HASH_ADD_INT(list_of_bidders, price, ht);
        } 
        insert(ht->h, bid);

        /* Find it's position in the sorted linked list and insert there */
        bid_node_t *temp = bid_list;
        while(temp->price != -1) {
                if(temp->price == b->price)
                        break;
                if(temp->next->price < b->price && temp->price > b->price){
                        bid_node_t *node = (bid_node_t *)malloc(sizeof(bid_node_t));
                        node->price = b->price;
                        node->next = temp->next;
                        temp->next = node;
                        break;
                }
                temp = temp->next;
        }
}

/* 
 * Remove offers from the data structures. Remove the price offering from
 * the linked list and from the hashtable as well.
 */
void remove_bids(int bidprice)
{
        hash_t *ht;
        HASH_FIND_INT(list_of_bidders, &bidprice, ht);
        HASH_DEL(list_of_bidders, ht);

        free(ht->h->heap);
        free(ht->h);
        free(ht);
        bid_node_t *temp = bid_list;

        while(temp->price != -1) {
                if(temp->next->price == bidprice){
                        temp->next = temp->next->next;
                        break;
                }
        }

        if (bidprice == higest_bid_price)
                higest_bid_price = bid_list->next->price;
}

hash_t *get_bidders(int bidprice)
{
        hash_t *ht;
        HASH_FIND_INT(list_of_bidders, &bidprice, ht);
        return ht;
}

bid_node_t *get_highest_bidding_price(int offerprice)
{
        if (higest_bid_price >= offerprice)
                return bid_list->next;
        else
                return NULL;
}

/* 
 * Add an unprocessed offer. Create a heap if not already there and add it
 * to linked list as well
 */
void add_offer(Order **offer)
{
        Order *b = *offer;
        if (b->price < lowest_offer_price)
                lowest_offer_price = b->price;

        hash_t *ht;
        HASH_FIND_INT(list_of_offers, &(b->price), ht);
        if(ht == NULL) {
                ht = (hash_t *)malloc(sizeof(hash_t));
                ht->price = b->price;
                ht->h = init_heap();
                HASH_ADD_INT(list_of_offers, price, ht);
        } 
        insert(ht->h, offer);

        /* Find it's position in the sorted linked list and insert there */
        offer_node_t *temp = offer_list;
        while(temp->price != 20000) {
                if(temp->price == b->price)
                        break;
                if(temp->next->price > b->price && temp->price < b->price){
                        offer_node_t *node = (offer_node_t *)malloc(sizeof(offer_node_t));
                        node->price = b->price;
                        node->next = temp->next;
                        temp->next = node;
                        break;
                }
                temp = temp->next;
        }
}

/* 
 * Remove offers from the data structures. Remove the price offering from
 * the linked list and from the hashtable as well.
 */
void remove_offers(int offerprice)
{
        hash_t *ht;
        HASH_FIND_INT(list_of_offers, &offerprice, ht);
        HASH_DEL(list_of_offers, ht);

        free(ht->h->heap);
        free(ht->h);
        free(ht);

        bid_node_t *temp = offer_list;
        while(temp->price != -20000) {
                if(temp->next->price == offerprice){
                        temp->next = temp->next->next;
                        break;
                }
        }

        if (offerprice == lowest_offer_price)
                lowest_offer_price = offer_list->next->price;
}

hash_t *get_offers(int offerprice)
{
        hash_t *ht;
        HASH_FIND_INT(list_of_offers, &offerprice, ht);
        return ht;
}

offer_node_t *get_lowest_offer_price(int bidprice)
{
        if (lowest_offer_price <= bidprice)
                return offer_list->next;
        else
                return NULL;
}

/* 
 * When a trade occurs,  update trader_shares for the person.
 * If the person bought it then add it otherwise subtract the shares 
 */
void update_trader_shares(char* name, int num, bool is_bid)
{
        shares_t *s = NULL;
        HASH_FIND_STR(trader_shares, name, s);
        if (s == NULL) {
                s = (shares_t *)malloc(sizeof(shares_t));
                strcpy(s->name, name);
                if(is_bid == true)
                        s->shares = num;
                else
                        s->shares = -1*num;
                HASH_ADD_STR(trader_shares, name, s);
        } else {
                if (is_bid == true)
                        s->shares = num + s->shares;
                else
                        s->shares = -1*num + s->shares;
        }
}

/* 
 * Get number of shares for a person based on the long and SHORT logic
 */
int get_num_shares(const char* name)
{
        shares_t *s;
        HASH_FIND_STR(trader_shares, name, s);
        return s == NULL ? 0 : s->shares;
}


/* 
 * Print shares for all the people based on the LONG and SHORT logic
 * as defined in the problem. Useful for debugging as well
 */
void print_ht() 
{
        shares_t *t;
        for(t=trader_shares; t != NULL; t=(shares_t *)(t->hh.next)) {
                printf("Name %s: Shares: %d\n", t->name, t->shares);
        }
}
