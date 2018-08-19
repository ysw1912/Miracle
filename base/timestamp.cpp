/*************************************************************************
    > File Name: timestamp.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-20
*************************************************************************/

#include "timestamp.h"

#include <stdio.h>
#include <sys/time.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS 

#include <boost/static_assert.hpp>

namespace Miracle
{
    BOOST_STATIC_ASSERT(sizeof(timestamp) == sizeof(int64_t));

    timestamp::timestamp()
        : m_usec_since_epoch(0) { }

    timestamp::timestamp(int64_t usec_since_epoch)
        : m_usec_since_epoch(usec_since_epoch) { }

    void timestamp::swap(timestamp& rhs)
    { std::swap(m_usec_since_epoch, rhs.m_usec_since_epoch); }

    std::string timestamp::to_string() const 
    {
        char buf[32] = {0};
        int64_t sec = m_usec_since_epoch / USEC_PER_SEC;
        int64_t usec = m_usec_since_epoch % USEC_PER_SEC;
        snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", sec, usec);
        return buf;
    }

    std::string timestamp::to_string_format() const 
    {
        char buf[32] = {0};
        time_t sec = static_cast<time_t>(m_usec_since_epoch / USEC_PER_SEC);
        int usec = static_cast<int>(m_usec_since_epoch % USEC_PER_SEC);
        struct tm tm_time;
        // gmtime_r(&sec, &tm_time);    // UTC时间
        localtime_r(&sec, &tm_time);    // 本时区时间
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                 usec);
        return buf;
    }

    timestamp timestamp::now()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return timestamp(tv.tv_sec * USEC_PER_SEC + tv.tv_usec);
    }
}
