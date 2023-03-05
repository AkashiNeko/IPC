#include "cmd.hpp"

int main() {

    // create pipe
    if (mkfifo(IPC_PATH, MODE) < 0) {
        perror("mkfifo");
        exit(1);
    }
    log("Create FIFO", Debug) << std::endl;

    // open pipe
    int fd = open(IPC_PATH, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(2);
    }
    log("Open FIFO", Debug) << std::endl;
    
    // -------------------- communication ----------------------
    
    char buffer[SIZE];
    while (true) {
        memset(buffer, 0, SIZE);
        ssize_t s = read(fd, buffer, SIZE - 1);
        if (s > 0) {
            // Read
            log("Receive", Notice) << "Message: " << buffer << std::endl;
        }
        else if (s == 0) {
            // End Of File
            log("Read EOF", Warning) << "Client quit, Server quit too." << std::endl;
            break;
        }
        else {
            // Read Error
            perror("Read");
        }
    }

    // ---------------------------------------------------------
    
    close(fd);
    log("Close FIFO", Debug) << std::endl;

    // remove pipe
    unlink(IPC_PATH);
    log("Unlink FIFO", Debug) << std::endl;
    return 0;
}