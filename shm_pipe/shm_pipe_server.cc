#include "cmd.hpp"

Init init;

int main() {

    // get key
    key_t k = ftok(PATH_NAME, PROJ_ID);
    if (k == -1) {
        log("create key failed", Error) << std::endl;
        exit(errno);
    }
    log("create key done", Debug) << "key: " << k << std::endl;

    // get share memory
    int shmid = shmget(k, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (shmid == -1) {
        if (errno == 17)
            log("shm exists", Error) << "The share memory is exist, use 'ipcrm' to remove it.." << std::endl;
        else
            perror("shmget");
        exit(errno);
    }
    log("create shm done", Debug) << "shmid: " << shmid << std::endl;

    // attach (share memory --> address)
    void* shmaddr = (void*)shmat(shmid, nullptr, 0);
    log("attach shm done", Debug) << "shmid: " << shmid << std::endl;

    // -------------------- communication ----------------------

    int fd = openFIFO(READ);

    char* msg = (char*)shmaddr;
    memset(msg, 0, SHM_SIZE);

    while (true) {
        log("waitting", Notice) << "wait for client.." << std::endl;
        wait(fd);
        if (strcmp(msg, "quit") == 0) {
            // quit
            log("Quit", Warning) << "Server quit" << std::endl;
            break;
        }
        else {
            log("Receive ", Notice) << "Message: " << msg << std::endl;
            memset(msg, 0, strlen(msg) + 1);
        }
    }

    // ---------------------------------------------------------

    // detach (share memory X-> address)
    int n = shmdt(shmaddr);
    assert(n != -1);
    log("detach shm done", Debug) << "shmid: " << shmid << std::endl;

    // remove share memory
    n = shmctl(shmid, IPC_RMID, nullptr);
    assert(n != -1);
    (void)n;
    log("delete shm done", Debug) << "shmid: " << shmid << std::endl;
    return 0;
}