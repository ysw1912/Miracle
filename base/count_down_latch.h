#ifndef MIRACLE_BASE_COUNT_DOWN_LATCH_H_
#define MIRACLE_BASE_COUNT_DOWN_LATCH_H_ 

#include "condition.h"
#include "mutex.h"

#include <boost/noncopyable.hpp>

namespace Miracle 
{
    class count_down_latch : boost::noncopyable 
    {
    public:
        explicit count_down_latch(int count);

        void wait();

        void count_down();

        int get_count() const;

    private:
        mutable lock    m_lock;
        condition       m_cond;
        int             m_count;
    };
}

#endif
