/*************************************************************************
    > File Name: timer_queue.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-31
*************************************************************************/

#include "event_loop.h"
#include "timer_queue.h"

#include <assert.h>
#include <string.h>
#include <sys/timerfd.h>

namespace Miracle
{
    namespace detail
    {
        int create_timerfd()
        {
            int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
            if (timerfd < 0) {
                printf("fatal: failed in timerfd_create\n");
            }
            return timerfd;
        }

        struct timespec timespec_from_now(timestamp when)
        {
            int64_t usec = when.usec_since_epoch() - timestamp::now().usec_since_epoch();
            if (usec < 100) {
                usec = 100;
            }
            struct timespec ts;
            ts.tv_sec = static_cast<time_t>(usec / timestamp::USEC_PER_SEC);
            ts.tv_nsec = static_cast<long>((usec % timestamp::USEC_PER_SEC) * 1000);
            return ts;
        }

        void read_timerfd(int timerfd, timestamp now)
        {
            uint64_t bytes;
            ssize_t n = ::read(timerfd, &bytes, sizeof(bytes));
            printf("trace: timer_queue::handle_read() %lu at %s\n", bytes, now.to_string().c_str());
            if (n != sizeof(bytes)) {
                printf("error: timer_queue::handle_read() reads %lu bytes instead of 8\n", n);
            }
        }

        void reset_timerfd(int timerfd, timestamp expire)
        {
            struct itimerspec newval;
            struct itimerspec oldval;
            bzero(&newval, sizeof(newval));
            bzero(&oldval, sizeof(oldval));
            newval.it_value = timespec_from_now(expire);
            int ret = ::timerfd_settime(timerfd, 0, &newval, &oldval);
            if (ret) {
                printf("syserr: timerfd_settime()\n");
            }
        }

    }

    timer_queue::timer_queue(event_loop* loop)
        : m_loop(loop),
          m_timerfd(detail::create_timerfd()),
          m_timerfd_channel(m_loop, m_timerfd),
          m_timers()
    {
        m_timerfd_channel.set_read_callback(std::bind(&timer_queue::handle_read, this));
        m_timerfd_channel.enable_read();
    }

    timer_queue::~timer_queue()
    {
        ::close(m_timerfd);
        for (timer_list::iterator it = m_timers.begin(); it != m_timers.end(); ++it) {
            delete it->second;
        }
    }

    timer* timer_queue::add_timer(const timer_callback& cb,
                                  timestamp when,
                                  double interval)
    {
        timer* t = new timer(cb, when, interval);
        m_loop->assert_in_loop_thread();
        bool earliest_changed = insert(t); 
        if (earliest_changed) {
            detail::reset_timerfd(m_timerfd, t->expire());
        }
        return t;
    }

    void timer_queue::handle_read()
    {
        m_loop->assert_in_loop_thread();
        timestamp now(timestamp::now());
        detail::read_timerfd(m_timerfd, now);

        std::vector<entry> expired = get_expired(now);

        for (auto it = expired.begin(); it != expired.end(); ++it) {
            it->second->run();
        }

        reset(expired, now);
    }

    std::vector<timer_queue::entry> timer_queue::get_expired(timestamp now)
    {
        std::vector<entry> expired;
        entry sentry = std::make_pair(now, reinterpret_cast<timer*>(UINTPTR_MAX));
        timer_list::iterator it = m_timers.lower_bound(sentry);
        assert(it == m_timers.end() || now < it->first);
        std::copy(m_timers.begin(), it, back_inserter(expired));
        m_timers.erase(m_timers.begin(), it);
        return expired;
    }

    void timer_queue::reset(const std::vector<entry>& expired, timestamp now)
    {
        timestamp next_expire;

        for (auto it = expired.begin(); it != expired.end(); ++it) {
            if (it->second->repeat()) {
                it->second->restart(now);
                insert(it->second);
            }
            else {
                delete it->second;
            }
        }

        if (!m_timers.empty()) {
            next_expire = m_timers.begin()->second->expire();
        }

        if (next_expire.is_valid()) {
            detail::reset_timerfd(m_timerfd, next_expire);
        }
    }

    bool timer_queue::insert(timer* t)
    {
        bool earliest_changed = false;
        timestamp when = t->expire();
        timer_list::iterator it = m_timers.begin();
        if (it == m_timers.end() || when < it->first) {
            earliest_changed = true;
        }
        assert(m_timers.insert(std::make_pair(when, t)).second);
        return earliest_changed;
    }
}


