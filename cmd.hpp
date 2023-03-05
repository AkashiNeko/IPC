#pragma once

#include <cstdio>
#include <cstring>

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "log.hpp"

using namespace std;

string ipcPath = "./fifo.ipc";

#define MODE 0666
#define SIZE 128