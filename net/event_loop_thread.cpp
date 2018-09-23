/*************************************************************************
    > File Name: event_loop_thread.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-09-24
*************************************************************************/

#include "event_loop.h"
#include "event_loop_thread.h"

namespace Miracle
{
    event_loop_thread::event_loop_thread()
        : m_loop(nullptr),
          m_quit(false),
          m_thread(std::bind(&event_loop_thread::thread_func, this)),
          m_lock(),
          m_cond(m_lock)
    { }

    event_loop_thread::~event_loop_thread()
    {
        m_quit = true;
        m_loop->quit();
        m_thread.join();
    }

    event_loop* event_loop_thread::start()
    {
        assert(!m_thread.started());

        m_thread.start();

        {
            lock_guard lg(m_lock);
            while (!m_loop) {
                m_cond.wait();
            }
        }

        return m_loop;
    }

    void event_loop_thread::thread_func()
    {
        event_loop loop;

        {
            lock_guard lg(m_lock);
            m_loop = &loop;
            m_cond.signal();
        }

        loop.loop();
    }
}
