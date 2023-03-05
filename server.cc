#include "cmd.hpp"

int main() {
    if (mkfifo(ipcPath.c_str(), MODE) < 0) {
        perror("mkfifo");
        exit(1);
    }
    log("Create FIFO", Debug) << endl;
    int fd = open(ipcPath.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(2);
    }
    log("Open FIFO", Debug) << endl;
    char buffer[SIZE] = {0};
    while (true) {
        memset(buffer, 0, SIZE);
        ssize_t s = read(fd, buffer, SIZE - 1);
        if (s > 0) {
            std::cout << "Receive: " << buffer << std::endl;
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
    unlink(ipcPath.c_str());
    log("Unlink FIFO", Debug) << endl;
    return 0;
}