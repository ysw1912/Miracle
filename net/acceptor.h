#ifndef MIRACLE_NET_ACCEPTOR_H_
#define MIRACLE_NET_ACCEPTOR_H_ 

#include "../base/common.h"
#include "socket.h"

#include <memory>

namespace Miracle 
{
    class inet_address;

    class tcp_stream;
    using tcp_stream_ptr = std::unique_ptr<tcp_stream>;

    class acceptor 
    {
    public:
        explicit acceptor(const inet_address& listen_addr);

        acceptor(acceptor&&) = default;
        ~acceptor() = default;
        acceptor& operator=(acceptor&&) = default;

        tcp_stream_ptr accept();

    private:
        socket m_listen_sock;
    };
}

#endif
