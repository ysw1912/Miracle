#ifndef MIRACLE_NET_SOCKET_H_
#define MIRACLE_NET_SOCKET_H_ 

namespace Miracle 
{

    class inet_address;

    class socket 
    {
    public:
        explicit socket(int sockfd);
        ~socket();

        // factory methods
        static socket create_tcp();
        static socket create_udp();

        int fd() { return m_sockfd; }

        void bind(const inet_address& addr);

    private:
        int m_sockfd;
    };
}

#endif
