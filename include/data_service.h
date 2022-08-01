
int process_request(int fd);
void parse_request(char *request, char *uri);
int is_file_exists(char* fname);
void send_not_found(int fd);
void send_data(int fd, char *filename);
