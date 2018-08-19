#ifndef MIRACLE_NET_SOCKET_H_
#define MIRACLE_NET_SOCKET_H_ 

#include <boost/noncopyable.hpp>

namespace Miracle 
{

    class inet_address;

    class socket : boost::noncopyable 
    {
    public:
        explicit socket(int sockfd);
        socket(socket&& rhs);

        ~socket();

        // factory methods
        static socket create_tcp();
        static socket create_udp();

        int fd() { return m_sockfd; }

        void bind(const inet_address& addr);
        void listen();

        // return 0 on success
        int connect(const inet_address& addr);

        int send(const void* buf, int len);
        int recv(void* buf, int len);

        void shutdown_write();

        void set_reuseaddr(bool on);
        void set_tcp_nodelay(bool on);

        inet_address get_local_addr() const;
        inet_address get_peer_addr() const;

    private:
        int m_sockfd;
    };
}

#endif
