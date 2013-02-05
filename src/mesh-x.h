#include<stdbool.h>
#include "types.h"

void init_orderbook();
void process_bid(Order **);
void process_offer(Order **);
int get_num_shares(const char*);
