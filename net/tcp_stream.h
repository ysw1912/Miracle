#ifndef MIRACLE_NET_TCP_STREAM_H_
#define MIRACLE_NET_TCP_STREAM_H_ 

#include "../base/common.h"
#include "socket.h"

#include <memory>

namespace Miracle 
{
    class inet_address;

    class tcp_stream;
    using tcp_stream_ptr = std::unique_ptr<tcp_stream>;

    class tcp_stream 
    {
    public:
        explicit tcp_stream(socket&& sock)
            : m_sock(std::move(sock)) {}

        tcp_stream(tcp_stream&&) = default;
        ~tcp_stream() = default;

        static tcp_stream_ptr connect(const inet_address& server_addr);
        static tcp_stream_ptr connect(const inet_address& server_addr, const inet_address& local_addr); 

        int recvall(void* buf, int len);
        int recv(void* buf, int len);

        int sendall(const void* buf, int len);
        int send(const void* buf, int len);

        void shutdown_write();

    private:
        static tcp_stream_ptr connect_aux(const inet_address& server_addr, const inet_address* local_addr);

    public:
        socket m_sock;
    };
}

#endif
