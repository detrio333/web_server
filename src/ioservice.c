#include <sys/epoll.h>
#include <unistd.h>
#include <pthread.h>

#include "ioservice.h"
#include "logger.h"
#include "server_lowlvl.h"
#include "data_service.h"

#define MAX_EPOLL_EVENTS_PER_RUN 25
#define EPOLL_RUN_TIMEOUT -1 

static int epoll_fd;
int ioservice_init(){
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        log_perror("epoll_create");
        return -1;
    }
    return 0;
}

void ioservice_close(){
    if (close(epoll_fd) == -1) {
        log_perror("close epoll");
    }
}

int ioservice_add(int fd){
    if (set_nonblock(fd) == -1) {
        log_perror("set_nonblock");
        return -1;
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    int s = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if (s == -1) {
        log_perror("epoll_ctl");
        return -1;
    }
    return 0;
}

int ioservice_create_worker(){
    pthread_t pthread_id;
    if (pthread_create(&pthread_id, 
                        NULL, 
                        &ioservice_thread_loop, 
                        NULL) != 0){
        log_perror("pthread_create");
        return -1;
    }
    if (pthread_detach(pthread_id) != 0) {
        log_perror("pthread_detach");
        return -1;
    }
    return 0;
}

void *ioservice_thread_loop(void *arg) {
    struct epoll_event events[MAX_EPOLL_EVENTS_PER_RUN];
    while(1){
        int n_fds = epoll_wait(epoll_fd, events, 
                                        MAX_EPOLL_EVENTS_PER_RUN,
                                        EPOLL_RUN_TIMEOUT);
        if (n_fds < 0) {
            //thread_safe error msg
        }
        int i;
        for (i = 0; i < n_fds; i++) {
            int fd = events[i].data.fd;
            if (process_request(fd) == -1) {
                //tread safe error msg
            }
        }
    }
}
