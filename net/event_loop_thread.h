#ifndef MIRACLE_EVENT_LOOP_THREAD_H_
#define MIRACLE_EVENT_LOOP_THREAD_H_ 

#include "../base/condition.h"
#include "../base/mutex.h"
#include "../base/thread.h"

#include <boost/noncopyable.hpp>

namespace Miracle 
{
    class event_loop;

    class event_loop_thread : boost::noncopyable 
    {
    public:
        event_loop_thread();
        ~event_loop_thread();
        event_loop* start();

    private:
        void thread_func();

    private:
        event_loop* m_loop;
        bool        m_quit;
        thread      m_thread;
        lock        m_lock;
        condition   m_cond;
    };
}

#endif
