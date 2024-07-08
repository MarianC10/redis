#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <aio.h>
#include <errno.h>
#include "common.hpp"

extern size_t k_max_msg;

static int32_t one_request(int connfd) {
    // 4 bytes header;
    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_all(connfd, rbuf, 4);
    if (err) {
        if (errno == 0) {
            error("EOF");
        }
        else {
            error("read() error");
        }
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf, 4); // assume little endian

    if (len > k_max_msg) {
        error("message too long");
        return -1;
    }

    err = read_all(connfd, rbuf + 4, len);
    if (err) {
        error("read error");
        return err;
    }

    rbuf[4 + len] = '\0';
    printf("client says: %s\n", rbuf + 4);

    const char reply[] = "world";
    char wbuf[4 + sizeof(reply)];
    len = (uint32_t) strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(wbuf + 4, reply, sizeof(reply));
    
    return write_all(connfd, wbuf, 4 + len);
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

        while (true) {
            int32_t err = one_request(connfd);
            if (err) {
                break;
            }
        }

        close(connfd);
    }

    return 0;
}