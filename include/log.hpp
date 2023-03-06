#pragma once

#include <iostream>
#include <ctime>

#define Debug 0
#define Notice 1
#define Warning 2
#define Error 3

std::string msg[] = {
    "Debug  ",
    "Notice ",
    "Warning",
    "Error  "
};

std::ostream& log(std::string message, int level) {
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    std::cout << " | " << tmp << " | " << msg[level] << " | " << message << " | ";
    return std::cout;
}