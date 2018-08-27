/*************************************************************************
    > File Name: thread.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-23
*************************************************************************/

#include "timestamp.h"
#include "thread.h"

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <sys/prctl.h>
#include <sys/syscall.h>

namespace Miracle
{
    namespace detail
    {
        pid_t gettid()
        {
            return static_cast<pid_t>(::syscall(SYS_gettid));
        }

        void after_fork()
        {
            current_thread::t_cached_tid = 0;
            current_thread::t_thread_name = "main";
            current_thread::tid(); 
        }

        class thread_name_init
        {
        public:
            thread_name_init()
            {
                current_thread::t_thread_name = "main";
                current_thread::tid();
                pthread_atfork(NULL, NULL, &after_fork);
            }
        };

        thread_name_init init;

        struct thread_data
        {
            using thread_func = Miracle::thread::thread_func;
            pid_t*          m_tid;
            std::string     m_name;
            thread_func     m_func;

            thread_data(pid_t* tid,
                        const std::string& name,
                        const thread_func& func)
                : m_tid(tid),
                  m_name(name),
                  m_func(func)
            { }

            void run()
            {
                *m_tid = current_thread::tid();
                m_tid = nullptr;

                current_thread::t_thread_name = m_name.empty() ? "miracle_thread" : m_name.c_str();
                ::prctl(PR_SET_NAME, current_thread::t_thread_name);
                try {
                    m_func();
                    current_thread::t_thread_name = "finished";
                }
                catch (const std::exception& ex) {
                    current_thread::t_thread_name = "crashed";
                    fprintf(stderr, "exception caught in thread %s\n", m_name.c_str());
                    fprintf(stderr, "reason: %s\n", ex.what());
                    abort();
                }
                catch (...) {
                    current_thread::t_thread_name = "crashed";
                    fprintf(stderr, "unknown exception caught in thread %s\n", m_name.c_str());
                    throw;
                }
            }
        };

        void* start_thread(void* arg)
        {
            thread_data* data = static_cast<thread_data*>(arg);
            data->run();
            delete data;
            return NULL;
        }
    }

    namespace current_thread
    {
        __thread int            t_cached_tid = 0;
        __thread char           t_tidstr[32];
        __thread int            t_tidstr_len = 6;
        __thread const char*    t_thread_name = "unknown";
        const bool same_type = boost::is_same<int, pid_t>::value;
        BOOST_STATIC_ASSERT(same_type);

        void cache_tid()
        {
            if (t_cached_tid == 0) {
                t_cached_tid = detail::gettid();
            }
        }

        bool is_main_thread()
        {
            return tid() == ::getpid();
        }

        void sleep_usec(int64_t usec)
        {
            struct timespec ts = {0, 0};
            ts.tv_sec = static_cast<time_t>(usec / timestamp::USEC_PER_SEC);
            ts.tv_nsec = static_cast<long>(usec % timestamp::USEC_PER_SEC * 1000);
            ::nanosleep(&ts, NULL);
        }
    }
    
    atomic_int32_t thread::num_created;  

    thread::thread(const thread_func& func, const std::string& name)
        : m_ptid(0),
          m_tid(0),
          m_name(name),
          m_func(func),
          m_start(false),
          m_join(false)
    {
        set_default_name();
    }

    thread::~thread()
    {
        if (m_start && !m_join) {
            pthread_detach(m_ptid);
        }
    }

    void thread::start()
    {
        assert(!m_start);
        m_start = true;
        detail::thread_data* data = new detail::thread_data(&m_tid, m_name, m_func);
        if (pthread_create(&m_ptid, NULL, &detail::start_thread, data)) {
            m_start = false;
            delete data;
            perror("failed in pthread_create");
        }
    }

    int thread::join()
    {
        assert(m_start);
        assert(!m_join);
        m_join = true;
        return pthread_join(m_ptid, NULL);
    }

    void thread::set_default_name()
    {
        int num = num_created.increment_and_get();
        if (m_name.empty()) {
            char buf[32];
            snprintf(buf, sizeof(buf), "thread%d", num);
            m_name = buf;
        }
    }
}
