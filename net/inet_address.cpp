/*************************************************************************
    > File Name: inetaddress.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-14
*************************************************************************/

#include "inet_address.h"

#include <assert.h>
#include <strings.h>    // bzero
#include <arpa/inet.h>  // inet_pton

using namespace Miracle;

inet_address::inet_address(String ip, uint16_t port)
    : inet_address(port)
{
    if (inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr) != 1)
        assert("Invalid ip format");
}

inet_address::inet_address(uint16_t port, bool loopback)
{
    bzero(&m_addr, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = htonl(loopback ? INADDR_LOOPBACK : INADDR_ANY);
    m_addr.sin_port = htons(port);
}

