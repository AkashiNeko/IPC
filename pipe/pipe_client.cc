#include "cmd.hpp"

int main() {
    int fd = open(IPC_PATH, O_WRONLY);
    if (fd < 0) {
        perror("open");
        exit(2);
    }
    while (true) {
        string buffer;
        std::cout << "Send Message: ";
        std::getline(std::cin, buffer);
        write(fd, buffer.c_str(), buffer.size());
    }
    close(fd);
    return 0;
}