/*************************************************************************
    > File Name: acceptor.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-19
*************************************************************************/

#include "acceptor.h"
#include "tcp_stream.h"

#include <sys/socket.h>

namespace Miracle
{
    acceptor::acceptor(const inet_address& listen_addr)
        : m_listen_sock(socket::create_tcp())
    {
        m_listen_sock.set_reuseaddr(true);
        m_listen_sock.bind(listen_addr);
        m_listen_sock.listen();
    }

    tcp_stream_ptr acceptor::accept()
    {
        int sockfd = ::accept(m_listen_sock.fd(), NULL, NULL);
        if (sockfd >= 0) {
            return tcp_stream_ptr(new tcp_stream(socket(sockfd)));
        }
        else {
            perror("acceptor::accept");
            return tcp_stream_ptr();
        }
    }
}
