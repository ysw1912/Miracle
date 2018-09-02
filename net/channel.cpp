/*************************************************************************
    > File Name: channel.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-29
*************************************************************************/

#include "channel.h"
#include "event_loop.h"

#include <poll.h>

namespace Miracle
{
    const int channel::READ_EVENT   = POLLIN | POLLPRI;
    const int channel::WRITE_EVENT  = POLLOUT;
    const int channel::NONE_EVENT   = 0;

    channel::channel(event_loop* loop, int fd)
        : m_loop(loop),
          m_fd(fd),
          m_events(0),
          m_revents(0),
          m_index(-1)
    { }

    void channel::handle_event()
    {
        if (m_revents & POLLNVAL) {
            printf("warn: channel::handle_event() POLLNVAL\n");
        }
        if (m_revents & (POLLIN | POLLPRI | POLLRDHUP)) {
            if (m_read_callback) {
                m_read_callback();
            }
        }
        if (m_revents & POLLOUT) {
            if (m_write_callback)
                m_write_callback();
        }
        if (m_revents & (POLLERR | POLLNVAL)) {
            if (m_error_callback)
                m_error_callback();
        }
    }

    void channel::update()
    {
        m_loop->update_channel(this);
    }
}
