#include "types.h"
#include <stdbool.h>

void add_bid(Order **);
void remove_bids(int);
hash_t *get_bidders(int);
bid_node_t *get_highest_bidding_price(int);

void add_offer(Order **);
void remove_offers(int);
hash_t *get_offers(int);
offer_node_t *get_lowest_offer_price(int);

void update_trader_shares(char*, int, bool);
