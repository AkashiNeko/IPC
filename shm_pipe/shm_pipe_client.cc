#include "cmd.hpp"

int main() {

    // get key
    key_t k = ftok(PATH_NAME, PROJ_ID);
    if (k == -1) {
        log("create key failed", Error) << std::endl;
        exit(errno);
    }
    log("create key done", Debug) << "key: " << k << std::endl;

    // get share memory
    int shmid = shmget(k, SHM_SIZE, 0);
    if (shmid == -1) {
        perror("shmget");
        exit(errno);
    }
    log("create shm done", Debug) << "shmid: " << shmid << std::endl;

    // attach (share memory --> address)
    void* shmaddr = shmat(shmid, nullptr, 0);
    log("attach shm done", Debug) << "shmid: " << shmid << std::endl;

    // -------------------- communication ----------------------

    char* msg = (char*)shmaddr;

    int fd = openFIFO(WRITE);

    while (true) {
        std::cout << " | Send Message: ";
        std::string s;
        std::getline(std::cin, s);
        if (s.empty())
            continue;
        if (s.size() > SHM_SIZE - 1) {
            log("Message is too long", Error) << "Length: " << s.size() << std::endl;
            continue;
        }
        strcpy(msg, s.c_str());
        signal(fd);
        if (strcmp(msg, "quit") == 0) {
            log("Quit", Notice) << "Client quit" << std::endl;
            break;
        }
    }

    // ---------------------------------------------------------

    // detach (share memory X-> address)
    int n = shmdt(shmaddr);
    assert(n != -1);
    log("detach shm done", Debug) << "shmid: " << shmid << std::endl;

    return 0;
}