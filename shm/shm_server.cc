#include "cmd.hpp"

using namespace std;

int main() {
    key_t k = ftok(PATH_NAME, PROJ_ID);
    if (k == -1) {
        log("create key failed", Error) << endl;
        exit(1);
    }
    log("create key done", Debug) << "key: " << k << endl;

    int shmid = shmget(k, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    log("create shm done", Debug) << "shmid: " << shmid << endl;

    char* shmaddr = (char*)shmat(shmid, nullptr, 0);
    log("attach shm done", Debug) << "shmid: " << shmid << endl;

    int n = shmdt(shmaddr);
    assert(n != -1);
    log("detach shm done", Debug) << "shmid: " << shmid << endl;

    n = shmctl(shmid, IPC_RMID, nullptr);
    assert(n != -1);
    (void)n;
    log("delete shm done", Debug) << "shmid: " << shmid << endl;
    return 0;
}