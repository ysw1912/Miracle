#ifndef MIRACLE_BASE_MUTEX_H_
#define MIRACLE_BASE_MUTEX_H_ 

#include "thread.h"

#include <boost/noncopyable.hpp>

#include <pthread.h>

namespace Miracle 
{

    class lock : boost::noncopyable 
    {
    public:
        lock() : m_current_tid(0)
        {
            pthread_mutex_init(&m_mutex, NULL);
        }

        ~lock()
        {
            assert(m_current_tid == 0);
            pthread_mutex_destroy(&m_mutex);
        }

        bool is_locked_by_this_thread() const 
        {
            return m_current_tid == current_thread::tid();
        }

        void assert_locked() const 
        {
            assert(is_locked_by_this_thread());
        }

    private:    
        /**
         *  仅供lock_guard调用
         */ 
        friend class lock_guard;

        void acquire()
        {
            pthread_mutex_lock(&m_mutex);
            assign_tid();
        }

        void release()
        {
            unassign_tid();
            pthread_mutex_unlock(&m_mutex);
        }

        /**
         *  仅供condition调用
         */ 
        friend class condition;

        pthread_mutex_t* get_pthread_mutex()
        {
            return &m_mutex;
        }

    private:
        class unassign_guard : boost::noncopyable 
        {
        public:
            unassign_guard(lock& owner) : m_owner(owner)
            {
                m_owner.unassign_tid();
            }

            ~unassign_guard()
            {
                m_owner.assign_tid();
            }

        private:
            lock& m_owner;
        };
        
        void assign_tid()
        {
            m_current_tid = current_thread::tid();
        }

        void unassign_tid()
        {
            m_current_tid = 0;
        }

    private:
        pthread_mutex_t     m_mutex;
        pid_t               m_current_tid;
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

// 误用保护
#define lock_guard(x) error "missing lock_guard obj name"
}
#endif
