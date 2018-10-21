/*************************************************************************
    > File Name: socket.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-14
*************************************************************************/

#include "inet_address.h"
#include "socket.h"

#include <assert.h>
#include <fcntl.h>  // fcntl
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
    {
        assert(m_sockfd >= 0);
    }

    socket::~socket()
    {
        close(m_sockfd);
    }

    void socket::close(int sockfd)
    {
        if (::close(sockfd) < 0)
            printf("error: socket::close");
    }

    int socket::create_tcp(bool nonblock, bool cloexec)
    {
        int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
        assert(sockfd >= 0);
        if (nonblock)
            set_nonblock(sockfd);
        if (cloexec)
            set_cloexec(sockfd);
        return sockfd;
    }

    int socket::set_nonblock(int sockfd)
    {
        int flags = ::fcntl(sockfd, F_GETFL, 0);
        flags |= O_NONBLOCK;
        int ret = ::fcntl(sockfd, F_SETFL, flags);
        return ret;
    }

    int socket::set_cloexec(int sockfd)
    {
        int flags = ::fcntl(sockfd, F_GETFD, 0);
        flags |= FD_CLOEXEC;
        int ret = ::fcntl(sockfd, F_SETFD, flags);
        return ret;
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

    int socket::accept(inet_address* addr, bool nonblock, bool cloexec)
    {
        struct sockaddr_in saddr;
        socklen_t addrlen = sizeof(saddr);
        bzero(&saddr, addrlen);
        int connfd = ::accept(m_sockfd, sockaddr_cast(&saddr), &addrlen);
        if (connfd >= 0) {
            if (nonblock)
                set_nonblock(connfd);
            if (cloexec)
                set_cloexec(connfd);
            addr->set_sockaddr_in(saddr);
        }
        else {
            int save_errno = errno;
            switch (save_errno)
            {
                case EAGAIN:
                case EINTR:
                    errno = save_errno;
                    break;
                default:
                    printf("unknown error of ::accept %d\n", save_errno);
                    break;
            }
        }
        return connfd;
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
