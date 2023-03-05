#include "cmd.hpp"

int main() {
    if (mkfifo(IPC_PATH, MODE) < 0) {
        perror("mkfifo");
        exit(1);
    }
    log("Create FIFO", Debug) << endl;
    int fd = open(IPC_PATH, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(2);
    }
    log("Open FIFO", Debug) << endl;
    char buffer[SIZE];
    while (true) {
        memset(buffer, 0, SIZE);
        ssize_t s = read(fd, buffer, SIZE - 1);
        if (s > 0) {
            log("Receive", Notice) << "Message: " << buffer << endl;
        }
        else if (s == 0) {
            // End Of File
            log("Read EOF", Warning) << "Client quit, Server quit too." << endl;
            break;
        }
        else {
            // Read Error
            perror("Read");
        }
    }
    close(fd);
    log("Close FIFO", Debug) << endl;
    unlink(IPC_PATH);
    log("Unlink FIFO", Debug) << endl;
    return 0;
}