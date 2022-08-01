#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "logger.h"

#define LOCKFILE "server.lock"

static int serverlockfile_fd;

int single_server_guard_lock(){
    int lock_fd = open(LOCKFILE, O_RDWR | O_CREAT, 0640);
    if (lock_fd == -1){
        log_perror("guard lock, open");
        return -1;
    }
    if (lockf(lock_fd, F_TLOCK, 0) == -1){
        log_perror("guard lock, lockf");
        return -1;
    }
    char str[10];
    sprintf(str, "%d\n", getpid());
    if (write(lock_fd, str, strlen(str)) == -1) {
        log_perror("guard lock, write");
        return -1;
    }
    return lock_fd;
}

int single_server_guard_unlock(int lock_fd){
    if (lockf(lock_fd, F_ULOCK, 0) == -1){
        log_perror("guard unlock, lockf");
        return -1;
    }
    if (close(lock_fd) == -1) {
        log_perror("guard unlock, close");
        return -1;
    }
    if (unlink(LOCKFILE) == -1) {
        log_perror("guard unlock, unlink");
        return -1;
    }
    return 0;
}

int set_nonblock(int fd){
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}

int start_server(char *ip4_addr, int port) {
    if ((serverlockfile_fd = single_server_guard_lock()) == -1)
        return -1;
    int master_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (master_socket == -1) {
        log_perror("socket");
	return -1;
    }
    struct sockaddr_in socket_address;
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port);
    int s = inet_pton(AF_INET, ip4_addr, &socket_address.sin_addr); 
    if (s == 0) {
        log_message("inet_pton: invalid ip addr");
	return -1;
    } else if (s == -1) {
        log_perror("inet_pton");
        return -1;
    }
    if (bind(master_socket, (struct sockaddr*)&socket_address, 
	sizeof(socket_address)) != 0){
	log_perror("bind");
	return -1;
    }
    /*if (set_nonblock(master_socket) == -1) {
        perror("set_nonblock");
        return -1;
    }*/
    if (listen(master_socket, SOMAXCONN)) {
	log_perror("listen");
	return -1;
    }

    return master_socket;
}

int stop_server(int master_socket){
    int status = 0;
    if (shutdown(master_socket, SHUT_RDWR)){
        log_perror("shutdown");
        status = -1;
    }
    if (close(master_socket) == -1) {
        log_perror("close socket");
        status = -1;
    }
    if (single_server_guard_unlock(serverlockfile_fd) == -1) {
        status = -1;
    }
    return status;
}
