/*************************************************************************
    > File Name: netcat.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-18
*************************************************************************/

#include "../net/acceptor.h"
#include "../net/inet_address.h"
#include "../net/socket.h"
#include "../net/tcp_stream.h"

#include <thread>
#include <memory>

#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int write_n(int fd, const void* buf, int len)
{
    int nwrite = 0;
    while (nwrite < len) {
        int nw = ::write(fd, static_cast<const char*>(buf) + nwrite, len - nwrite);
        if (nw > 0) {
            nwrite += nw;
        }
        else if (nw == 0) {
            break;
        }
        else if (errno != EINTR) {
            perror("write");
            break;
        }
    }
    return nwrite;
}

void run(Miracle::tcp_stream_ptr stream)
{
    std::thread thr([&stream] () {
        char buf[8192];
        int nr = 0;
        while ((nr = stream->recv(buf, sizeof(buf))) > 0) {
            int nw = write_n(STDOUT_FILENO, buf, nr);
            if (nw < nr) {
                break;
            }
        }
        ::exit(0);
    });

    char buf[8192];
    int nr = 0;
    while ((nr = ::read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        int nw = stream->send_all(buf, nr);
        if (nw < nr) {
            break;
        }
    }
    stream->shutdown_write(); 
    thr.join();
}

void run_server(int port)
{
    std::unique_ptr<Miracle::acceptor> acceptor(new Miracle::acceptor(Miracle::inet_address(port)));
    Miracle::tcp_stream_ptr stream(acceptor->accept());
    if (stream) {
        acceptor.reset();
        run(std::move(stream));
    }
    else {
        perror("accept");
    }
}

void run_client(const char* hostname, int port)
{
    Miracle::inet_address server_addr(port);
    if (!Miracle::inet_address::resolve(hostname, &server_addr)) {
        printf("Unable to resolve %s\n", hostname);
        return;
    }

    Miracle::tcp_stream_ptr stream(Miracle::tcp_stream::connect(server_addr));
    if (stream) {
        run(std::move(stream));
    }
    else {
        printf("Unable to connect %s\n", hostname);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("Usage:\n  %s hostname port\n  %s -l port\n", argv[0], argv[0]);
        return 0;
    }
    int port = atoi(argv[2]);

    if (strcmp(argv[1], "-l") == 0) {
        run_server(port);
    }
    else {
        run_client(argv[1], port);
    }
    return 0;
}
