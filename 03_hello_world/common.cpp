#include "common.hpp"
#include <stdio.h>
#include <stdlib.h>

void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}