#include "common.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

void error(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

int32_t read_all(int fd, char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0) {
            return -1;
        }

        assert((size_t) rv <= n);

        n -= (size_t) rv;
        buf += rv;
    }

    return 0;
}

int32_t write_all(int fd, char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1;
        }

        assert((size_t) rv <= n);

        n -= (size_t) rv;
        buf += rv;
    }

    return 0;
}