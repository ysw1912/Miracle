/*************************************************************************
    > File Name: poll.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-30
*************************************************************************/

#include "channel.h"
#include "poll.h"

#include <assert.h>
#include <poll.h>

namespace Miracle
{
    poller::poller(event_loop* loop)
        : m_ownerloop(loop)
    { }

    poller::~poller()
    { }

    timestamp poller::poll(int timeout_ms, channel_list* active_channels)
    {
        int num_events = ::poll(m_pollfds.data(), m_pollfds.size(), timeout_ms);
        timestamp now(timestamp::now());
        if (num_events > 0) {
            printf("trace: poller::poll() %d events happened\n", num_events);
            fill_active_channels(num_events, active_channels);
        }
        else if (num_events == 0) {
            printf("trace: poller::poll() Nothing happened\n");
        }
        else {
            printf("syserr: poller::poll()\n");
        }
        return now;
    }

    void poller::update_channel(channel* ch)
    {
        assert_in_loop_thread();
        printf("trace: poller::update_channel() fd = %d, events = %d\n", ch->fd(), ch->events());
        if (ch->index() < 0) {
            assert(m_channels.find(ch->fd()) == m_channels.end());
            struct pollfd pfd;
            pfd.fd = ch->fd();
            pfd.events = static_cast<short>(ch->events());
            pfd.revents = 0;
            m_pollfds.push_back(pfd);
            int idx = static_cast<int>(m_pollfds.size()) - 1;
            ch->set_index(idx);
            m_channels[pfd.fd] = ch;
        }
        else {
            assert(m_channels.find(ch->fd()) != m_channels.end());
            assert(m_channels[ch->fd()] == ch);
            int idx = ch->index();
            assert(0 <= idx && idx < static_cast<int>(m_pollfds.size()));
            struct pollfd& pfd = m_pollfds[idx];
            assert(pfd.fd == ch->fd() || pfd.fd == -1);
            pfd.events = static_cast<short>(ch->events());
            pfd.revents = 0;
            if (ch->is_none_event()) {
                pfd.fd = -1;
            }
        }
    }

    void poller::fill_active_channels(int num_events, channel_list* active_channels) const 
    {
        for (pollfd_list::const_iterator pfd = m_pollfds.begin();
             pfd != m_pollfds.end() && num_events > 0;
             ++pfd)
        {
            if (pfd->revents > 0) {
                --num_events;
                channel_map::const_iterator map = m_channels.find(pfd->fd);
                assert(map != m_channels.end());
                channel* ch = map->second;
                assert(ch->fd() == pfd->fd);
                ch->set_revents(pfd->revents);
                active_channels->push_back(ch);
            }
        }
    }
}
