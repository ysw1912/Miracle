#ifndef MIRACLE_NET_ACCEPTOR_H_
#define MIRACLE_NET_ACCEPTOR_H_ 

#include "../base/common.h"
#include "channel.h"
#include "socket.h"

#include <boost/noncopyable.hpp>

namespace Miracle 
{
    class event_loop;
    class inet_address;

    class acceptor : boost::noncopyable 
    {
    public:
        using new_conn_cb = std::function<void(int connfd, const inet_address& peer_addr)>;

    public:
        acceptor(event_loop* loop, const inet_address& listen_addr);

        void set_new_connection_callback(const new_conn_cb& cb)
        {
            m_cb = cb;
        }

        bool listening() const 
        {
            return m_listening;
        }

        void listen();

    private:
        void handle_read();
    
    private:
        event_loop*     m_loop;
        socket          m_listen_sock;
        channel         m_channel;
        new_conn_cb     m_cb;
        bool            m_listening;
    };
}

#endif
