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
#include <netdb.h>

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

static const int BUFSIZE = 4096;

bool inet_address::resolve_slow(const char* hostname, inet_address* out)
{
    std::vector<char> buf(2 * BUFSIZE);
    struct hostent hent;
    struct hostent* he = NULL;
    int herrno = 0;
    bzero(&hent, sizeof(hent));

    while (buf.size() <= 16 * BUFSIZE) {
        int ret = gethostbyname_r(hostname, &hent, buf.data(), buf.size(), &he, &herrno);
        if (ret == 0 && he != NULL) {
            assert(he->h_addrtype == AF_INET && he->h_length == 4);
            out->m_addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
            return true;
        }
        else if (ret == ERANGE) {
            buf.resize(buf.size() * 2);
        }
        else {
            if (ret) {
                perror("inet_address::resolve");
            }
            return false;
        }
    }
    return false;
}

bool inet_address::resolve(String hostname, inet_address* out)
{
    assert(out);
    char buf[BUFSIZE];
    struct hostent hent;
    struct hostent* he = NULL;
    int herrno = 0;
    bzero(&hent, sizeof(hent));

    int ret = gethostbyname_r(hostname.c_str(), &hent, buf, sizeof(buf), &he, &herrno);
    if (ret == 0 && he != NULL) {
        assert(he->h_addrtype == AF_INET && he->h_length == 4);
        out->m_addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
        return true;
    }
    else if (ret == ERANGE) {
        return resolve_slow(hostname.c_str(), out);
    }
    else {
        if (ret) {
            perror("inet_address::resolve");
        }
        return false;
    }
}
