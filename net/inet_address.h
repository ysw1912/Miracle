#pragma once

#include "../base/common.h"

#include <string>
#include <vector>

#include <netinet/in.h>

namespace Miracle
{
    class inet_address
    {
    public:
        inet_address(String ip, uint16_t port);
        explicit inet_address(uint16_t port, bool loopback = false);    // for listening

        explicit inet_address(const struct sockaddr_in& addr)
            : m_addr(addr) {}
       
        const struct sockaddr_in&
        get_sockaddr_in() const 
        {
            return m_addr;
        } 

        void 
        set_sockaddr_in(const struct sockaddr_in& addr)
        {
            m_addr = addr;
        }

        bool operator==(const inet_address& rhs) const 
        {
            return m_addr.sin_family == rhs.m_addr.sin_family 
                && m_addr.sin_addr.s_addr == rhs.m_addr.sin_addr.s_addr 
                && m_addr.sin_port == rhs.m_addr.sin_port;
        }

        // "ip:port"的string
        std::string to_string() const;

    public:
        // 主机名解析为ip地址
        static bool resolve(String hostname, inet_address* out);

    private:
        static bool resolve_slow(const char* hostname, inet_address* out);
    private:
        struct sockaddr_in m_addr;
    };
}
