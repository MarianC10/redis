#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <aio.h>
#include "common.hpp"

static void do_something(int connfd) {
    char rbuf[64] = {};

    ssize_t n = read(connfd, rbuf, sizeof(rbuf - 1));
    if (n < 0) {
        fprintf(stderr, "Read error!");
        return;
    }
    else {
        printf("Bytes read from client: %ld\n", n);
    }

    printf("Client says: %s\n", rbuf);

    char wbuf[] = "world";
    n = write(connfd, wbuf, strlen(wbuf));
    if (n < 0) {
        fprintf(stderr, "Write error");
    }
}


int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_ANY);

    int rv = bind(fd, (const sockaddr *) &addr, sizeof(addr));
    if (rv) {
        die("Bind failed!");
    }

    rv = listen(fd, SOMAXCONN);
    if (rv) {
        die("Listen failed!");
    }

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *) &client_addr, &socklen);
        if (connfd < 0) {
            continue;
        }

        do_something(connfd);

        close(connfd);
    }

    return 0;
}