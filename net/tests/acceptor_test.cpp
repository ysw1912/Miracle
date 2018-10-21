/*************************************************************************
    > File Name: acceptor_test.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-10-21
*************************************************************************/

#include "../acceptor.h"
#include "../event_loop.h"
#include "../inet_address.h"

#include <iostream>

using namespace std;

void new_connection(int sockfd, const Miracle::inet_address& peer_addr)
{
    printf("new_connection(): accept a new connection from %s\n", "11");
    ::write(sockfd, "Hello World\n", 12);
    Miracle::socket::close(sockfd);
}

int main()
{
    printf("main(): pid = %d\n", getpid());

    Miracle::inet_address listen_addr(12345);
    Miracle::event_loop loop;

    Miracle::acceptor acceptor(&loop, listen_addr);
    acceptor.set_new_connection_callback(new_connection);
    acceptor.listen();

    loop.loop();
}
