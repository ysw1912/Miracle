/*************************************************************************
    > File Name: count_down_latch.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-27
*************************************************************************/

#include "count_down_latch.h"

namespace Miracle
{
    count_down_latch::count_down_latch(int count)
        : m_lock(),
          m_cond(m_lock),
          m_count(count)
    { }

    void count_down_latch::wait()
    {
        lock_guard lg(m_lock);
        while (m_count > 0) {
            m_cond.wait();
        }
    }

    void count_down_latch::count_down()
    {
        lock_guard lg(m_lock);
        --m_count;
        if (m_count == 0) {
            m_cond.broadcast();
        }
    }

    int count_down_latch::get_count() const 
    {
        lock_guard lg(m_lock);
        return m_count;
    }

}

