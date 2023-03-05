#pragma once

#include <iostream>
#include <ctime>

#define Debug 0
#define Notice 1
#define Warning 2
#define Error 3

std::string msg[] = {
    "Debug",
    "Notice",
    "Warning",
    "Error"
};

std::ostream& log(std::string message, int level) {
    std::cout << " | " << (unsigned int)time(nullptr) << " | " << msg[level] << " | " << message << " | ";
    return std::cout;
}