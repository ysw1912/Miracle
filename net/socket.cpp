/*************************************************************************
    > File Name: socket.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-14
*************************************************************************/

#include "inet_address.h"
#include "socket.h"

#include <assert.h>

using namespace Miracle;

struct sockaddr*
sockaddr_cast(struct sockaddr_in* saddr)
{
    return static_cast<struct sockaddr*>(implicit_cast<void*>(saddr));
}

const struct sockaddr*
sockaddr_cast(const struct sockaddr_in* saddr)
{
    return static_cast<const struct sockaddr*>(implicit_cast<const void*>(saddr));
}

socket::socket(int sockfd) : m_sockfd(sockfd)
{ assert(m_sockfd >= 0); }

socket::~socket()
{
    if (m_sockfd >= 0) {
        assert(::close(m_sockfd) == 0);
    }
}

Miracle::socket socket::create_tcp()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
    assert(sockfd >= 0);
    return socket(sockfd);
}

Miracle::socket socket::create_udp()
{
    int sockfd = ::socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
    assert(sockfd >= 0);
    return socket(sockfd);
}

void socket::bind(const inet_address& addr)
{
    const struct sockaddr_in& saddr = addr.get_sockaddr_in();
    int ret = ::bind(m_sockfd, sockaddr_cast(&saddr), sizeof(saddr));
    if (ret) {
        perror("socket::bind");
        abort();
    }
}
