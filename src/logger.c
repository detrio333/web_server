#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "logger.h"

#define LOGFILE "server.log"

void log_message(char *message) {
    FILE *logfile = fopen(LOGFILE, "a");
    if (logfile == NULL){
        perror("logger, fopen");
        return;
    }
    fprintf(logfile, "%s\n", message);
    fclose(logfile);
}

void log_perror(char *label) {
    char err_msg[255];
    int err_number = errno;
    errno = 0;
    sprintf(err_msg, "%s: %s", label, strerror(err_number));
    log_message(err_msg);
}
