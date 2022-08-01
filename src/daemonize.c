#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void close_opened_fds(){
    fflush(stdout);
    int i;
    for (i = getdtablesize(); i>= 0; --i)
            close(i);
    i = open("/dev/null", O_RDWR);
    dup(i);
    dup(i);
}

int daemonize(){
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    else if (pid > 0)
        return 0;

    if (setsid() == (pid_t)-1) {
        perror("setsid");
        return -1;
    }
    close_opened_fds();
    umask(027);
    return 1;
}
