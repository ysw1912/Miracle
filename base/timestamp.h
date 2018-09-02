#ifndef MIRACLE_BASE_TIMESTAMP_H_
#define MIRACLE_BASE_TIMESTAMP_H_ 

#include <string>

#include <stdint.h>

namespace Miracle 
{
    class timestamp 
    {
    public:
        timestamp();
        explicit timestamp(int64_t usec_since_epoch);

        void swap(timestamp& rhs);

        std::string to_string() const;
        std::string to_string_format() const;

        int64_t usec_since_epoch() const 
        {
            return m_usec_since_epoch;
        }

        bool is_valid() const 
        {
            return m_usec_since_epoch > 0;
        }

        static timestamp now();

        static const int USEC_PER_SEC = 1000000;

    private:
        int64_t m_usec_since_epoch;
    };

    inline bool operator<(timestamp lhs, timestamp rhs)
    {
        return lhs.usec_since_epoch() < rhs.usec_since_epoch();
    }

    inline bool operator==(timestamp lhs, timestamp rhs)
    {
        return lhs.usec_since_epoch() == rhs.usec_since_epoch();
    }

    // 时间差, 单位: s
    inline double sec_diff(timestamp start, timestamp end)
    {
        int64_t diff = end.usec_since_epoch() - start.usec_since_epoch();
        return static_cast<double>(diff) / timestamp::USEC_PER_SEC;
    }

    inline timestamp add_sec(timestamp ts, double sec)
    {
        int64_t delta = static_cast<int64_t>(sec * timestamp::USEC_PER_SEC);
        return timestamp(ts.usec_since_epoch() + delta);
    }
}

#endif
