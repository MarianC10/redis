#include <aio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "common.hpp"

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

    char wbuf[] = "hello";
    ssize_t n = write(fd, wbuf, strlen(wbuf));
    if (n < 0) {
        fprintf(stderr, "Write error!");
    }

    char rbuf[64] = {};

    n = read(fd, rbuf, sizeof(rbuf - 1));
    if (n < 0) {
        fprintf(stderr, "Read error!");
        return 0;
    }
    else {
        printf("Bytes read from server: %ld\n", n);
    }

    printf("Server says: %s\n", rbuf);

    return 0;
}