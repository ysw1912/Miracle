#ifndef MIRACLE_BASE_CONDITION_H_
#define MIRACLE_BASE_CONDITION_H_ 

#include "mutex.h"

#include <boost/noncopyable.hpp>

#include "errno.h"

namespace Miracle 
{
    class condition : boost::noncopyable 
    {
    public:
        explicit condition(lock& lock) : m_lock(lock)
        {
            pthread_cond_init(&m_pcond, NULL);
        }

        ~condition()
        {
            pthread_cond_destroy(&m_pcond);
        }

        void wait()
        {
            lock::unassign_guard ug(m_lock);
            pthread_cond_wait(&m_pcond, m_lock.get_pthread_mutex());
        }

        bool timedwait(double sec)
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            const int64_t NSEC_PER_SEC = 1000000000;
            int64_t nsec = static_cast<int64_t>(sec * NSEC_PER_SEC);
            ts.tv_sec += static_cast<time_t>((ts.tv_nsec + nsec) / NSEC_PER_SEC);
            ts.tv_nsec = static_cast<long>((ts.tv_nsec + nsec) % NSEC_PER_SEC);

            lock::unassign_guard ug(m_lock);
            return ETIMEDOUT == pthread_cond_timedwait(&m_pcond, m_lock.get_pthread_mutex(), &ts);
        }

        void signal()
        {
            pthread_cond_signal(&m_pcond);
        }

        void broadcast()
        {
            pthread_cond_broadcast(&m_pcond);
        }

    private:
        lock&           m_lock;
        pthread_cond_t  m_pcond;
    };
}

#endif
