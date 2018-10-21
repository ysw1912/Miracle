#ifndef MIRACLE_NET_CHANNEL_H_
#define MIRACLE_NET_CHANNEL_H_ 

#include <boost/noncopyable.hpp>
#include <functional>

namespace Miracle 
{
    class event_loop;

    class channel : boost::noncopyable 
    {
    public:
        using event_callback = std::function<void()>;

        channel(event_loop* loop, int fd);

        void handle_event();

        void set_read_callback(const event_callback& cb)
        {
            m_read_callback = cb;
        }

        void set_write_callback(const event_callback& cb)
        {
            m_write_callback = cb;
        }

        void set_error_callback(const event_callback& cb)
        {
            m_error_callback = cb;
        }

        event_loop* this_loop()
        {
            return m_loop;
        }

        int fd() const 
        {
            return m_fd;
        }
    
        int events() const 
        {
            return m_events;
        }

        int index()
        {
            return m_index;
        }

        void set_index(int idx)
        {
            m_index = idx;
        }

        void set_revents(int re)
        {
            m_revents = re;
        }

        bool is_none_event() const 
        {
            return m_events == NONE_EVENT;
        }

        void enable_read()
        {
            m_events |= READ_EVENT;
            update();
        }

    private:
       void update();

    private:
        static const int    READ_EVENT;
        static const int    WRITE_EVENT;
        static const int    NONE_EVENT;

        event_loop*     m_loop;
        const int       m_fd;    
        int             m_events;
        int             m_revents;
        int             m_index;

        event_callback  m_read_callback;
        event_callback  m_write_callback;
        event_callback  m_error_callback;
    };
}

#endif
