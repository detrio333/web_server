
int ioservice_init();
void ioservice_close();
int ioservice_add();

int ioservice_create_worker();
void *ioservice_thread_loop(void *arg);
