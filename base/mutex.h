#ifndef MIRACLE_BASE_MUTEX_H_
#define MIRACLE_BASE_MUTEX_H_ 

#include <boost/noncopyable.hpp>

#include <pthread.h>

namespace Miracle 
{

    class lock : boost::noncopyable 
    {
    public:
        lock()
        {
            pthread_mutex_init(&m_mutex, NULL);
        }

        ~lock()
        {
            pthread_mutex_destroy(&m_mutex);
        }

        void acquire()
        {
            pthread_mutex_lock(&m_mutex);
        }

        void release()
        {
            pthread_mutex_unlock(&m_mutex);
        }

    private:
        pthread_mutex_t m_mutex;
    };

    class lock_guard : boost::noncopyable 
    {
    public:
        explicit lock_guard(lock& lock) : m_lock(lock)
        { m_lock.acquire(); }

        ~lock_guard()
        { m_lock.release(); }

    private:
        lock& m_lock;
    };

}
#endif
