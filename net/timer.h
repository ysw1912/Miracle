#ifndef MIRACLE_NET_TIMER_H_
#define MIRACLE_NET_TIMER_H_ 

#include "../base/timestamp.h"

#include <boost/noncopyable.hpp>
#include <functional>

namespace Miracle 
{
    
    using timer_callback = std::function<void()>;
    
    class timer : boost::noncopyable 
    {
    public:
        
        timer(const timer_callback& cb, timestamp when, double interval)
            : m_callback(cb),
              m_expire(when),
              m_interval(interval),
              m_repeat(interval > 0.0)
        { }

        void run() const 
        {
            m_callback();
        }

        timestamp expire() const 
        {
            return m_expire;
        }

        bool repeat() const 
        {
            return m_repeat;
        }

        void restart(timestamp now)
        {
            if (m_repeat) {
                m_expire = now.add(m_interval);
            }
            else {
                m_expire = timestamp();
            }
        }

    private:
        const timer_callback    m_callback;
        timestamp               m_expire;
        const double            m_interval;
        const bool              m_repeat;
    };
}

#endif
