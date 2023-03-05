#include "cmd.hpp"

int main() {

    // open pipe
    int fd = open(IPC_PATH, O_WRONLY);
    if (fd < 0) {
        perror("open");
        exit(errno);
    }
    
    // -------------------- communication ----------------------
    
    while (true) {
        std::string buffer;
        std::cout << "Send Message: ";
        std::getline(std::cin, buffer);
        write(fd, buffer.c_str(), buffer.size());
    }

    // ---------------------------------------------------------

    close(fd);
    return 0;
}