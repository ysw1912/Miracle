#ifndef MIRACLE_BASE_THREAD_H_
#define MIRACLE_BASE_THREAD_H_ 

#include "atomic.h"

#include <boost/noncopyable.hpp>
#include <memory>

#include <pthread.h>

namespace Miracle 
{
    namespace this_thread
    {
        extern __thread int         t_tid;
        extern __thread char        t_tidstr[32];
        extern __thread int         t_tidstr_len;
        extern __thread const char* t_thread_name;

        void cache_tid();

        inline int id()
        {
            // t_cachedtid == 0的可能性很小
            if (__builtin_expect(t_tid == 0, 0)) {
                cache_tid();
            }
            return t_tid;
        }

        inline const char* name()
        {
            return t_thread_name;
        }

        bool is_main_thread();

        void sleep_usec(int64_t usec);
    }

    class thread : boost::noncopyable 
    {
    public:
        using thread_func = std::function<void()>;

        explicit thread(const thread_func&, const std::string& name = std::string());

        ~thread();

        void start();
        int join();

        pid_t id() const 
        {
            return m_tid;
        }

        const std::string& name() const 
        {
            return m_name;
        }

        bool started() const 
        {
            return m_start;
        }

    private:
        void set_default_name();

    private:
        pthread_t               m_ptid;
        pid_t                   m_tid;
        std::string             m_name;
        thread_func             m_func;
        bool                    m_start;
        bool                    m_join;

        static atomic_int32_t   num_created;
    };
}

#endif
