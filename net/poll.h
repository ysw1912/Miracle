#ifndef MIRACLE_NET_POLL_H_
#define MIRACLE_NET_POLL_H_ 

#include "event_loop.h"
#include "../base/timestamp.h"

#include <boost/noncopyable.hpp>
#include <map> 
#include <vector>

struct pollfd;

namespace Miracle 
{
    class channel;

    class poller : boost::noncopyable 
    {
    public:
        using channel_list = std::vector<channel*>;

        poller(event_loop* loop);
        ~poller();

        timestamp poll(int timeout_ms, channel_list* active_channels);

        void update_channel(channel* ch);

        void assert_in_loop_thread()
        {
            m_ownerloop->assert_in_loop_thread();
        }

    private:
        void fill_active_channels(int num_events, channel_list* active_channels) const;

    private:
        using pollfd_list = std::vector<struct pollfd>;
        using channel_map = std::map<int, channel*>;

        event_loop*     m_ownerloop;
        pollfd_list     m_pollfds;
        channel_map     m_channels;
    };
}

#endif
