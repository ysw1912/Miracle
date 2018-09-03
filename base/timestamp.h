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

        bool operator<(timestamp rhs) const 
        {
            return m_usec_since_epoch < rhs.m_usec_since_epoch;
        }

        bool operator==(timestamp rhs) const 
        {
            return m_usec_since_epoch == rhs.m_usec_since_epoch;
        }

        timestamp add(double sec) const 
        {
            int64_t delta = static_cast<int64_t>(sec * timestamp::USEC_PER_SEC);
            return timestamp(m_usec_since_epoch + delta);
        }

        double operator-(timestamp rhs) const 
        {
            int64_t diff = m_usec_since_epoch - rhs.m_usec_since_epoch;
            return static_cast<double>(diff) / timestamp::USEC_PER_SEC;
        }

    private:
        int64_t m_usec_since_epoch;
    };

}

#endif
