#pragma once

#include <iostream>
#include <cstdio>
#include <cassert>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "log.hpp"

#define PATH_NAME "/home"
#define PROJ_ID 0x114514
#define SHM_SIZE 4096