#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "arguments.h"

void print_usage(char *prog_name){
    fprintf(stderr, "Usage: %s -h <ip> -p <port> -d <directory>\n", prog_name);
}

int get_opt_args(int argc, char **argv, char *ip4_addr, int *port, char *work_dir) {
    if (argc != 7) {
        print_usage(argv[0]);
        return 1;
    }
    int opt = 0;
    while ((opt = getopt(argc, argv, "h:p:d:")) != -1) {
        switch (opt) {
            case 'h':
                strcpy(ip4_addr, optarg);
                break;
            case 'p':
                *port = atoi(optarg);
                break;
            case 'd':
                strcpy(work_dir, optarg);
                break;
            default:
                print_usage(argv[0]);
                return 1;
            }
        
    }
    return 0;
}
