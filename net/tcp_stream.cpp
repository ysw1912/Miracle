/*************************************************************************
    > File Name: tcp_stream.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-18
*************************************************************************/

#include "inet_address.h"
#include "tcp_stream.h"

#include <signal.h>

namespace Miracle 
{
    
    class ignore_sigpipe
    {
    public:
        ignore_sigpipe()
        {
            ::signal(SIGPIPE, SIG_IGN);
        }
    } init_obj;

    bool is_self_connection(const socket& sock)
    {
        return sock.get_local_addr() == sock.get_peer_addr();
    }

    tcp_stream_ptr tcp_stream::connect_aux(const inet_address& server_addr, const inet_address* local_addr)
    {
        tcp_stream_ptr stream;
        socket sock(socket::create_tcp());
        if (local_addr) {
            sock.bind(*local_addr);
        }
        if (sock.connect(server_addr) == 0 && !is_self_connection(sock)) {
            stream.reset(new tcp_stream(std::move(sock)));
        }
        return stream;
    }

    tcp_stream_ptr tcp_stream::connect(const inet_address& server_addr)
    {
        return connect_aux(server_addr, nullptr);
    }

    tcp_stream_ptr tcp_stream::connect(const inet_address& server_addr, const inet_address& local_addr)
    {
        return connect_aux(server_addr, &local_addr);
    }

    int tcp_stream::recv_all(void* buf, int len)
    {
        return ::recv(m_sock.fd(), buf, len, MSG_WAITALL);
    }

    int tcp_stream::recv(void* buf, int len)
    {
        return m_sock.recv(buf, len);
    }

    int tcp_stream::send_all(const void* buf, int len)
    {
        int nwrite = 0;
        while (nwrite < len) {
            int nw = m_sock.send(static_cast<const char*>(buf) + nwrite, len - nwrite);
            if (nw > 0) {
                nwrite += nw;
            }
            else if (nw == 0) {
                break;
            }
            else if (errno != EINTR) {
                break;
            }
        }
        return nwrite;
    }

    int tcp_stream::send(const void* buf, int len)
    {
        return m_sock.send(buf, len);
    }

    void tcp_stream::shutdown_write() 
    {
        m_sock.shutdown_write();
    }

}
