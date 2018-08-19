/*************************************************************************
    > File Name: socket.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-14
*************************************************************************/

#include "inet_address.h"
#include "socket.h"

#include <assert.h>
#include <stdio.h>  // perror
#include <stdlib.h> // abort
#include <unistd.h> // close
#include <strings.h>
#include <netinet/tcp.h>    // TCP_NODELAY

namespace Miracle {
    
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

    socket::socket(socket&& rhs) : socket(rhs.m_sockfd)
    { rhs.m_sockfd = -1; }

    socket::~socket()
    {
        if (m_sockfd >= 0) {
            assert(::close(m_sockfd) == 0);
        }
    }

    socket socket::create_tcp()
    {
        int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
        assert(sockfd >= 0);
        return socket(sockfd);
    }

    socket socket::create_udp()
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

    void socket::listen()
    {
        int ret = ::listen(m_sockfd, SOMAXCONN);
        if (ret) {
            perror("socket::listen");
            abort();
        }
    }

    int socket::connect(const inet_address& addr)
    {
        const struct sockaddr_in& saddr = addr.get_sockaddr_in();
        return ::connect(m_sockfd, sockaddr_cast(&saddr), sizeof(saddr));
    }

    int socket::send(const void* buf, int len)
    {
        return ::send(m_sockfd, buf, len, 0);
    }

    int socket::recv(void* buf, int len)
    {
        return ::recv(m_sockfd, buf, len, 0);
    }

    void socket::shutdown_write()
    {
        if (::shutdown(m_sockfd, SHUT_WR) < 0) {
            perror("socket::shutdown_write");
        }
    }

    void socket::set_reuseaddr(bool on)
    {
        int optval = on ? 1 : 0;
        if (::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
            perror("socket::set_reuseaddr");
        }
    }

    void socket::set_tcp_nodelay(bool on)
    {
        int optval = on ? 1 : 0;
        if (::setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)) < 0) {
            perror("socket::set_tcp_nodelay");
        }
    }

    inet_address socket::get_local_addr() const 
    {
        struct sockaddr_in local_addr;
        bzero(&local_addr, sizeof(local_addr));
        socklen_t addrlen = sizeof(local_addr);
        if (::getsockname(m_sockfd, sockaddr_cast(&local_addr), &addrlen) < 0) {
            perror("socket::get_local_addr");
        }
        return inet_address(local_addr);
    }

    inet_address socket::get_peer_addr() const 
    {
        struct sockaddr_in peer_addr;
        bzero(&peer_addr, sizeof(peer_addr));
        socklen_t addrlen = sizeof(peer_addr);
        if (::getpeername(m_sockfd, sockaddr_cast(&peer_addr), &addrlen) < 0) {
            perror("socket::get_peer_addr");
        }
        return inet_address(peer_addr);
    }

}
