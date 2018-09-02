#ifndef MIRACLE_NET_TIMER_QUEUE_H_
#define MIRACLE_NET_TIMER_QUEUE_H_ 

#include "../base/timestamp.h"
#include "channel.h"
#include "timer.h"

#include <boost/noncopyable.hpp>
#include <set>
#include <vector>

namespace Miracle 
{
    class event_loop;

    class timer_queue : boost::noncopyable 
    {
    public:

        timer_queue(event_loop*);
        ~timer_queue();

        timer* add_timer(const timer_callback&, timestamp, double);

    private:

        using entry         = std::pair<timestamp, timer*>;
        using timer_list    = std::set<entry>;

        void add_timer_in_loop(timer* t);

        void handle_read();

        std::vector<entry> get_expired(timestamp now);

        void reset(const std::vector<entry>& expired, timestamp now);

        bool insert(timer* t);

    private:

        event_loop* m_loop;
        const int   m_timerfd;
        channel     m_timerfd_channel;
        timer_list  m_timers;

    };
}


#endif
