/*************************************************************************
    > File Name: acceptor.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-19
*************************************************************************/

#include "acceptor.h"
#include "event_loop.h"
#include "inet_address.h"

#include <sys/socket.h>

namespace Miracle
{
    acceptor::acceptor(event_loop* loop, const inet_address& listen_addr)
        : m_loop(loop),
          m_listen_sock(socket::create_tcp()),
          m_channel(loop, m_listen_sock.fd()),
          m_listening(false)
    {
        m_listen_sock.set_reuseaddr(true);
        m_listen_sock.bind(listen_addr);
        m_channel.set_read_callback(std::bind(&acceptor::handle_read, this));
    }

    void acceptor::listen()
    {
        m_loop->assert_in_loop_thread();
        m_listening = true;
        m_listen_sock.listen();
        m_channel.enable_read();
    }

    void acceptor::handle_read()
    {
        m_loop->assert_in_loop_thread();
        inet_address peer_addr(0);
        int connfd = m_listen_sock.accept(&peer_addr);
        if (connfd >= 0) {
            if (m_cb)
                m_cb(connfd, peer_addr);
            else 
                socket::close(connfd);                
        }
    }
}
