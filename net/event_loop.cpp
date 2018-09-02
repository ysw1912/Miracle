/*************************************************************************
    > File Name: event_loop.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-29
*************************************************************************/

#include "channel.h"
#include "event_loop.h"
#include "poll.h"
#include "timer_queue.h"

#include <assert.h>
#include <poll.h>

namespace Miracle
{
    __thread event_loop* t_loop_in_this_thread = nullptr;
    const int POLLTIME_MS = 10000;

    event_loop::event_loop()
        : m_looping(false),
          m_quit(false),
          m_tid(this_thread::id()),
          m_poller(new poller(this)),
          m_timer_queue(new timer_queue(this))
    {
        printf("trace: event_loop ctor [%p] in thread %d\n", this, m_tid);
        if (t_loop_in_this_thread) {
            printf("fatal: another event_loop [%p] exists in this thread %d\n",
                   t_loop_in_this_thread, m_tid);
        }
        else {
            t_loop_in_this_thread = this;
        }
    }

    event_loop::~event_loop()
    {
        assert(!m_looping);
        t_loop_in_this_thread = nullptr;
    }

    void event_loop::loop()
    {
        assert(!m_looping);
        assert_in_loop_thread();

        m_looping = true;
        m_quit = false;
        
        while (!m_quit) {
            m_active_channels.clear();
            m_poller->poll(POLLTIME_MS, &m_active_channels);
            for (channel_list::iterator ch = m_active_channels.begin();
                 ch != m_active_channels.end();
                 ++ch) {
                (*ch)->handle_event();
            }
        }
        printf("trace: event_loop [%p] stop looping\n", this);
        m_looping = false;
    }

    void event_loop::quit()
    {
        m_quit = true;
    }

    void event_loop::run_in_loop(const Functor& cb)
    {
        if (is_in_loop_thread()) {
            cb();
        }
    }

    void event_loop::queue_in_loop(const Functor& cb)
    {

    }

    timer* event_loop::run_at(const timestamp& time, const timer_callback& cb)
    {
        return m_timer_queue->add_timer(cb, time, 0.0); 
    }

    timer* event_loop::run_after(double delay, const timer_callback& cb)
    {
        timestamp time(add_sec(timestamp::now(), delay));
        return run_at(time, cb); 
    }

    timer* event_loop::run_every(double interval, const timer_callback& cb)
    {
        timestamp time(add_sec(timestamp::now(), interval));
        return m_timer_queue->add_timer(cb, time, interval); 
    }

    event_loop* event_loop::get_event_loop_of_this_thread()
    {
        return t_loop_in_this_thread;
    }

    void event_loop::update_channel(channel* ch)
    {
        assert(ch->this_loop() == this);
        assert_in_loop_thread();
        m_poller->update_channel(ch);
    }

    void event_loop::abort_not_in_loop_thread()
    {
        printf("fatal: event_loop [%p] was created in thread %d, this thread id is %d\n",
                this, m_tid, this_thread::id());
    }
}
