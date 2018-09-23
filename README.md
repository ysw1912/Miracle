学习muduo库，写网络库，用于学习网络编程。

### base

- 时间戳封装`base/timestamp.{h,cpp}`

- lock-free 的 atomic整数`base/atomic.h`

底层线程同步原语的封装

- 线程的创建和等待结束，封装为`base/thread.{h,cpp}`
- 互斥锁的创建、销毁、加锁、解锁，封装为`base/mutex.h`
- 条件变量的创建、销毁、等待、通知、广播，封装为`base/condition.h`

高层封装

- 用互斥锁和条件变量，封装为倒计时`base/count_down_latch.h`

### net

Reactor模式的事件循环封装为`event_loop.{h,cpp}`

基于one loop per thread，一个带有事件循环的线程封装为`event_loop_thread.{h, cpp}`

IO复用封装`poll.{h,cpp}`

tcp连接的socket API封装`socket.{h,cpp}`等
