#include <aio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include "common.hpp"

extern size_t k_max_msg;

static int32_t query(int fd, const char *text) {
    uint32_t len = (uint32_t)strlen(text);
    if (len < k_max_msg) {
        return -1;
    }

    char wbuf[4 + len];
    memcpy(wbuf, &len, 4);
    memcpy(wbuf + 4, text, len);

    int32_t err = write_all(fd, wbuf, 4 + len);
    if (err) {
        return err;
    }

    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_all(fd, rbuf, 4);
    if (err) {
        if (errno == 0) {
            error("EOF");
        } else {
            error("read() error");
        }
        return err;
    }

    memcpy(&len, rbuf, 4);
    if (len > k_max_msg) {
        error("message too long");
        return -1;
    }

    err = read_all(fd, rbuf + 4, len);
    if (err) {
        error("read() error");
        return err;
    }

    rbuf[4 + len] = '\0';
    printf("server says: %s\n", rbuf + 4);
    return 0;
}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("Failed to create socket!");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);

    int rv = connect(fd, (sockaddr *) &addr, sizeof(addr));
    if (rv < 0) {
        die("Failed to connect to server!");
    }

    int32_t err = query(fd, "hello1");
    if (err) {
        goto LD_DONE;
    }

    err = query(fd, "hello2");
    if (err) {
        goto LD_DONE;
    }

    err = query(fd, "hello3");
    if (err) {
        goto LD_DONE;
    }

LD_DONE:
    close(fd);
    return 0;
}