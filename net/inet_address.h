#pragma once

#include "common.h"

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
        { return m_addr; } 

        void 
        set_sockaddr_in(const struct sockaddr_in& addr)
        { m_addr = addr; }

    private:
        struct sockaddr_in m_addr;
    };
}
