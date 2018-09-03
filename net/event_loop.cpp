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
#include <sys/eventfd.h>

namespace Miracle
{
    __thread event_loop* t_loop_in_this_thread = nullptr;
    const int POLLTIME_MS = 10000;

    static int create_eventfd()
    {
        int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0) {
            printf("syserr: failed in eventfd\n");
            abort();
        }
        return evtfd;
    }

    event_loop::event_loop()
        : m_looping(false),
          m_quit(false),
          m_calling_pending_functors(false),
          m_tid(this_thread::id()),
          m_poller(new poller(this)),
          m_timer_queue(new timer_queue(this)),
          m_wakeupfd(create_eventfd()),
          m_wakeup_channel(new channel(this, m_wakeupfd))
    {
        printf("trace: event_loop [%p] ctor in thread %d\n", this, m_tid);
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
        ::close(m_wakeupfd);
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
            m_poll_return_time = m_poller->poll(POLLTIME_MS, &m_active_channels);
            for (channel_list::iterator ch = m_active_channels.begin();
                 ch != m_active_channels.end();
                 ++ch) {
                (*ch)->handle_event();
            }
            do_pending_functors();
        }
        printf("trace: event_loop [%p] stop looping\n", this);
        m_looping = false;
    }

    void event_loop::quit()
    {
        m_quit = true;
        if (!is_in_loop_thread()) {
            wakeup();
        }
    }

    void event_loop::run_in_loop(const Functor& cb)
    {
        // printf("run_in_loop()\n");
        if (is_in_loop_thread()) {
            // printf("in loop thread\n");
            cb();
        }
        else {
            // printf("not in loop thread\n");
            queue_in_loop(cb);
        }
    }

    void event_loop::queue_in_loop(const Functor& cb)
    {
        {
            lock_guard lg(m_mutex);
            m_pending_functors.push_back(cb);
        }
        if (!is_in_loop_thread() || m_calling_pending_functors) {
            wakeup();
        }
    }

    timer* event_loop::run_at(const timestamp& time, const timer_callback& cb)
    {
        return m_timer_queue->add_timer(cb, time, 0.0); 
    }

    timer* event_loop::run_after(double delay, const timer_callback& cb)
    {
        timestamp time(timestamp::now().add(delay));
        return run_at(time, cb); 
    }

    timer* event_loop::run_every(double interval, const timer_callback& cb)
    {
        timestamp time(timestamp::now().add(interval));
        return m_timer_queue->add_timer(cb, time, interval); 
    }

    event_loop* event_loop::get_event_loop_of_this_thread()
    {
        return t_loop_in_this_thread;
    }

    void event_loop::wakeup()
    {
        uint64_t one = 1;
        ssize_t n = ::write(m_wakeupfd, &one, sizeof(one));
        if (n != sizeof(one)) {
            printf("error: event_loop::wakeup() writes %ld bytes instead of 8\n", n);
        }
    }

    void event_loop::update_channel(channel* ch)
    {
        assert(ch->this_loop() == this);
        assert_in_loop_thread();
        m_poller->update_channel(ch);
    }

    void event_loop::handle_read()
    {
        uint64_t one;
        ssize_t n = ::read(m_wakeupfd, &one, sizeof(one));
        if (n != sizeof(one)) {
            printf("error: event_loop::handle_read() reads %ld bytes instead of 8\n", n);
        }
    }

    void event_loop::abort_not_in_loop_thread()
    {
        printf("fatal: event_loop [%p] was created in thread %d, this thread id is %d\n",
                this, m_tid, this_thread::id());
        abort();
    }

    void event_loop::do_pending_functors()
    {
        // printf("do_pending_functors()\n");
        std::vector<Functor> functors;
        m_calling_pending_functors = true;

        {
            lock_guard lg(m_mutex);
            functors.swap(m_pending_functors);
        }

        for (size_t i = 0; i < functors.size(); ++i) {
            functors[i]();
        }

        m_calling_pending_functors = false;
    }
}
