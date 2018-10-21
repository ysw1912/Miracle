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
        ~socket();

    public:
        static void close(int sockfd);

        static int create_tcp(bool nonblock = true, bool cloexec = true);
        static int set_nonblock(int sockfd);
        static int set_cloexec(int sockfd);

    public:
        int fd() const
        {
            return m_sockfd;
        }

        void bind(const inet_address& addr);
        void listen();
        int accept(inet_address* addr, bool nonblock = true, bool cloexec = true);

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
        const int m_sockfd;
    };
}

#endif
