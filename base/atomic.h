#ifndef MIRACLE_BASE_ATOMIC_H_
#define MIRACLE_BASE_ATOMIC_H_ 

#include <boost/noncopyable.hpp>
#include <stdint.h>

namespace Miracle 
{
    template <typename T>
    class atomic_integer : boost::noncopyable 
    {
    public:
        atomic_integer() : m_val(0) {}

        // 若 val == 0(参数二), 则 val = 0(参数三)
        // 返回操作前的值
        T get() const 
        { return __sync_val_compare_and_swap(const_cast<volatile T*>(&m_val), 0, 0); }

        // val += x, 返回 val 的旧值
        T get_and_add(T x)
        { return __sync_fetch_and_add(&m_val, x); }

        // val += x, 返回 val 
        T add_and_get(T x)
        { return __sync_add_and_fetch(&m_val, x); }

        T increment_and_get()
        { return add_and_get(1); }

        void add(T x)
        { get_and_add(x); }

        void increment()
        { increment_and_get(); }

        void decrement()
        { get_and_add(-1); }

        // val = new_val, 返回 val 的旧值
        T get_and_set(T new_val)
        { return __sync_lock_test_and_set(&m_val, new_val); }

    private:
        volatile T m_val;
    };

    using atomic_int32_t = atomic_integer<int32_t>;
    using atomic_int64_t = atomic_integer<int64_t>;
}

#endif
