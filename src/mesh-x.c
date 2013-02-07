#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include"mesh-x.h"
#include<unistd.h>

/* The path to orders.csv will be this if executed as ./mesh-x */
#define ORDER_FILE "../ext/orders.csv" 

/* 
 * The path to orders.csv will be this if executed as ./bin/mesh-x.
 * Don't want to write getcwd and check the working directory right now
 */
#define ALTERNATE_ORDER_FILE "ext/orders.csv"
#define MAX_LINE_LEN 100

void process_input_file(FILE *fp);

int main(int argc, const char* argv[])
{
        FILE *fp;
        if(((fp = fopen(ORDER_FILE, "r")) == NULL) && 
                        ((fp = fopen(ALTERNATE_ORDER_FILE, "r")) == NULL)) {
                fprintf(stderr, "Cannot open the file\n");
                exit(1);
        }

        init_orderbook();
        process_input_file(fp);
        int shares = get_num_shares("Kaylee");
        printf("%d\n", shares);
        fclose(fp);
        return 0;
}

void process_input_file(FILE *fp)
{
        Order *order;
        char line[MAX_LINE_LEN];
        char *split;

        int i = 0;
        while((fgets(line, MAX_LINE_LEN-2, fp)) != NULL) {
                /* Each line is in the following format
                 * 70000001,Mal,73.21,100,100001,BUY */
                split = strtok(line, ",");
                while(split != NULL) {  
                        i = i%6;
                        switch(i){
                                case 0:
                                        order = (Order *)malloc(sizeof(Order));
                                        break;
                                case 1:
                                        strcpy(order->name, split);
                                        break;
                                case 2: 
                                        order->price = (100*atof(split))+0.5;
                                        break;
                                case 3:
                                        order->quantity = atoi(split);
                                        break;
                                case 4:
                                        order->timestamp = atoi(split);
                                        break;
                                case 5:
                                        order->is_bid = (strcmp(split,"BUY\n") == 0) ? true:false;
                                        if(order->is_bid)
                                                process_bid(&order);
                                        else
                                                process_offer(&order);
                                        break;
                        }
                        split = strtok(NULL, ",");
                        ++i;
                }
        }
}
