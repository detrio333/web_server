#include <linux/limits.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "arguments.h"
#include "logger.h"
#include "server.h"


int main(int argc, char **argv) {
    char ip4_addr[15];
    int port;
    char work_dir[PATH_MAX];

    if (get_opt_args(argc, argv, ip4_addr, &port, work_dir)){
        exit(EXIT_FAILURE);
    }
    if (chdir(work_dir) == -1) {
        perror("chdir");
        exit(EXIT_FAILURE);
    }

    server_run(ip4_addr, port);

    return EXIT_SUCCESS;
}
