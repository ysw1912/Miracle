/*************************************************************************
    > File Name: chargen.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-20
*************************************************************************/

#include "../base/atomic.h"
#include "../base/timestamp.h"
#include "../net/acceptor.h"
#include "../net/inet_address.h"
#include "../net/tcp_stream.h"

#include <string.h>

#include <string>
#include <thread>

using Miracle::inet_address;
using Miracle::tcp_stream;
using Miracle::tcp_stream_ptr;

Miracle::atomic_int64_t g_bytes;

std::string get_message()
{
    std::string line;
    for (int i = 33; i < 127; ++i) {
        line.push_back(char(i));
    }
    line += line;

    std::string message;
    for (size_t i = 0; i < 127 - 33; ++i) {
        message += line.substr(i, 72) + '\n';
    }
    return message;
}

void measure()
{
    using Miracle::timestamp;
    timestamp start = timestamp::now();
    while (true) {
        struct timespec ts = {1, 0};
        ::nanosleep(&ts, NULL);

        // not atomic 
        int64_t bytes = g_bytes.get_and_set(0);
        timestamp end = timestamp::now();
        
        double elapsed = Miracle::sec_diff(start, end);
        start = end;
        if (bytes) {
            printf("%.3f MiB/s\n", bytes / (1024.0 * 1024) / elapsed);
        }
    }
}

void chargen(Miracle::tcp_stream_ptr stream)
{
    std::string message = get_message();
    while (true) {
        int nw = stream->sendall(message.data(), message.size()); 
        g_bytes.add(nw);
        if (nw < static_cast<int>(message.size())) {
            break;
        }
    }
}

void run_server(int port) 
{
    Miracle::acceptor acceptor{inet_address(port)};
    printf("Accepting... Ctrl-C to exit\n");
    int count = 0;
    while (true) {
        tcp_stream_ptr stream = acceptor.accept();
        printf("accepted No.%d client\n", ++count);
        std::thread(chargen, std::move(stream)).detach();
    }
}

void run_client(const char* hostname, int port)
{
    inet_address server_addr(port);
    if (!inet_address::resolve(hostname, &server_addr)) {
        printf("Unable to resolve %s\n", hostname);
        return;
    }

    tcp_stream_ptr stream(tcp_stream::connect(server_addr));
    if (stream) {
        chargen(std::move(stream));
    }
    else {
        printf("Unable to connect %s\n", hostname);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Usage:\n  %s hostname port\n  %s -l port\n", argv[0], argv[0]);
        return 0;
    }

    std::thread(measure).detach();

    int port = atoi(argv[2]);
    if (strcmp(argv[1], "-l") == 0) {
        run_server(port);
    }
    else {
        run_client(argv[1], port);
    }
    return 0;
}
