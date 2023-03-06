#pragma once

#include <iostream>
#include <string>

#include <cstdio>
#include <cstring>
#include <cassert>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <fcntl.h>
#include <unistd.h>

#include "log.hpp"

#define IPC_PATH "fifo.ipc"

#define MODE 0666
#define SIZE 128

#define PATH_NAME "/home"
#define PROJ_ID 0x114514
#define SHM_SIZE 4096

class Init {
public:
    Init() {
        umask(0);
        int n = mkfifo(IPC_PATH, 0666);
        assert(n == 0);
        (void)n;
        log("Create FIFO", Debug) << std::endl;
    }
    ~Init() {
        unlink(IPC_PATH);
        log("Remove FIFO", Debug) << std::endl;
    }
};

#define READ O_RDONLY
#define WRITE O_WRONLY

int openFIFO(int flags) {
    int fd = open(IPC_PATH, flags);
    assert(fd >= 0);
    return fd;
}

void wait(int fd) {
    uint32_t temp = 0;
    ssize_t s = read(fd, &temp, sizeof(uint32_t));
    assert(s == sizeof(uint32_t));
    (void)s;
}

void signal(int fd) {
    uint32_t temp = 1;
    ssize_t s = write(fd, &temp, sizeof(uint32_t));
    assert(s == sizeof(uint32_t));
    (void)s;
}

void closeFIFO(int fd) {
    close(fd);
}