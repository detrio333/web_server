#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "data_service.h"
#define REQ_MAX_LENGTH 8192
#define FILEBUF_MAX_SIZE 16000
int process_request(int fd){
    char buffer[REQ_MAX_LENGTH];
    int recv_res = recv(fd, buffer, REQ_MAX_LENGTH, MSG_NOSIGNAL);
    if (recv_res == 0 && errno != EAGAIN) {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
    else if (recv_res > 0 ) {
        //log_message("new incoming socket");
        //log_message(buffer);
	char req_uri[REQ_MAX_LENGTH];
        parse_request(buffer, req_uri);
        //log_message(req_uri);
	if (is_file_exists(req_uri)){
	    send_data(fd, req_uri);
	}
	else {
	    send_not_found(fd);
	}
    }
    return 0;
}

void parse_request(char *req, char *req_uri) {
    req_uri[0] = '.';
    sscanf(req, "GET %s HTTP/1.0", req_uri+1);
    char *attr = strchr(req_uri, '?');
    if (attr != NULL) {
        req_uri[attr - req_uri] = '\0';
    }
}

void send_data(int fd, char *filename) {
    static char ok_header[] = "HTTP/1.0 200 OK\r\n\r\n";
    char filedata[FILEBUF_MAX_SIZE];
    size_t header_size = strlen(ok_header);
    strcpy(filedata, ok_header);
    int file_fd = open(filename, O_RDONLY);
    ssize_t bytes = read(file_fd, filedata + header_size, FILEBUF_MAX_SIZE-header_size);
    close(file_fd);
    send(fd, filedata, strlen(filedata), MSG_NOSIGNAL);
    shutdown(fd, SHUT_RDWR);
    close(fd);
}

void send_not_found(int fd){
    static char notfound[] = 
"HTTP/1.0 404 Not Found\r\nContent-Length: 0\r\nContent-Type: text/html\r\n\r\n";
    send(fd, notfound, strlen(notfound), MSG_NOSIGNAL);
    shutdown(fd, SHUT_RDWR);
    close(fd);
}

int is_file_exists(char* fname){
    if (access(fname, F_OK) != -1) 
	return 1;
    return 0;
}
