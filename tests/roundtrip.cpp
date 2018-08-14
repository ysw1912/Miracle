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
}

void run_client()
{
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
        run_client();
    }
    return 0;
}
