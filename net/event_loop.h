#ifndef MIRACLE_NET_EVENT_LOOP_H_
#define MIRACLE_NET_EVENT_LOOP_H_ 

#include "../base/mutex.h"
#include "../base/thread.h"
#include "timer.h"

#include <boost/noncopyable.hpp>
#include <vector>

namespace Miracle 
{
    
    class channel;
    class poller;
    class timer_queue;

    class event_loop : boost::noncopyable 
    {
    public:
        using Functor = std::function<void()>;

        event_loop();
        ~event_loop();

        void loop();
        void quit();

        void run_in_loop(const Functor& cb);
        void queue_in_loop(const Functor& cb);

        timer* run_at(const timestamp& time, const timer_callback& cb);
        timer* run_after(double delay, const timer_callback& cb);
        timer* run_every(double interval, const timer_callback& cb); 

        bool is_in_loop_thread() const 
        {
            return m_tid == this_thread::id();
        }

        void assert_in_loop_thread()
        {
            if (!is_in_loop_thread()) {
                abort_not_in_loop_thread();
            }
        }

        timestamp poll_return_time() const 
        {
            return m_poll_return_time;
        }

        static event_loop* get_event_loop_of_this_thread();

        void wakeup();

        // 仅供channel::update()调用
        void update_channel(channel* ch);

    private:
        void abort_not_in_loop_thread();
        
        void handle_read();

        void do_pending_functors();

    private:
        using channel_list = std::vector<channel*>;

        bool                                m_looping;
        bool                                m_quit;
        bool                                m_calling_pending_functors;
        const pid_t                         m_tid;
        const std::unique_ptr<poller>       m_poller;
        timestamp                           m_poll_return_time; 
        const std::unique_ptr<timer_queue>  m_timer_queue;
        int                                 m_wakeupfd;
        const std::unique_ptr<channel>      m_wakeup_channel;
        channel_list                        m_active_channels;
        lock                                m_mutex;
        std::vector<Functor>                m_pending_functors;
    };
}

#endif
