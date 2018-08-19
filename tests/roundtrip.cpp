/*************************************************************************
    > File Name: roundtrip.c
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-14
*************************************************************************/

#include "../net/inet_address.h"
#include "../net/socket.h"

#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

const int g_port = 12345;

struct Message
{
    int64_t request;
    int64_t response;
} __attribute__ ((__packed__));

static_assert(sizeof(Message) == 16, "Message size should be 16 bytes");

int64_t now()
{
    struct timeval tv = {0, 0};
    gettimeofday(&tv, NULL);
    return tv.tv_sec * int64_t(1000000) + tv.tv_usec;
}

void run_server()
{
    Miracle::socket sock(Miracle::socket::create_udp());
    sock.bind(Miracle::inet_address(g_port));

    while (true) {
        Message message = {0, 0};

        struct sockaddr peer_addr;
        bzero(&peer_addr, sizeof(peer_addr));
        socklen_t addrlen = sizeof(peer_addr);
        ssize_t nr = ::recvfrom(sock.fd(), &message, sizeof(message), 0, &peer_addr, &addrlen);
        if (nr == sizeof(message))
        {
            message.response = now();
            ssize_t nw = ::sendto(sock.fd(), &message, sizeof(message), 0, &peer_addr, addrlen);
            if (nw < 0) {
                perror("send Message");
            }
            else if (nw != sizeof(message)) {
                printf("send Message of %zd bytes, expect %zd bytes\n", nw, sizeof(message));
            }
        }
        else if (nr < 0) {
            perror("recv Message");
        }
        else {
            printf("recv Message of %zd bytes, expect %zd bytes\n", nr, sizeof(message));
        }
    }
}

void run_client(const char* hostname)
{
    Miracle::socket sock(Miracle::socket::create_udp());
    Miracle::inet_address server_addr(g_port);
    if (!Miracle::inet_address::resolve(hostname, &server_addr)) {
        printf("Unable to resolve %s\n", hostname);
        return;
    }
    if (sock.connect(server_addr) != 0) {
        perror("connect to server");
        return;
    }

    std::thread thr([&sock]() {
        while (true) {
            Message message = {0, 0};
            message.request = now();
            int nw = sock.send(&message, sizeof(message));
            if (nw < 0) {
                perror("send Message");
            }
            else if (nw != sizeof(message)) {
                printf("send Message of %d bytes, expect %zd bytes\n", nw, sizeof(message));
            }
            ::usleep(200 * 1000);
        }
    });

    while (true) {
        Message message = {0, 0};
        int nr = sock.recv(&message, sizeof(message));
        if (nr == sizeof(message)) {
            int64_t back = now();
            int64_t mine = (back + message.request) / 2;
            printf("now %jd, round trip %jd, clock error %jd\n",
                   back, back - message.request, message.response - mine);
        }
        else if (nr < 0) {
            perror("receive Message");
        }
        else {
            printf("receive Message of %d bytes, expect %zd bytes\n", nr, sizeof(message));
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage:\nServer: %s -s\nClient: %s hostname\n", argv[0], argv[0]);
        return 0;
    }
    if (strcmp(argv[1], "-s") == 0) {
        run_server();
    }
    else {
        run_client(argv[1]);
    }
    return 0;
}
