/*************************************************************************
    > File Name: event_loop_test.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-29
*************************************************************************/

#include "../event_loop.h"
#include "../channel.h"

#include <iostream>

#include <sys/timerfd.h>
#include <string.h>

int count = 0;
Miracle::event_loop* g_loop;

void print_tid()
{
    printf("pid = %d, tid = %d\n", getpid(), Miracle::this_thread::id());
    printf("now %s\n", Miracle::timestamp::now().to_string_format().c_str());
}

void print(const char* msg)
{
    printf("msg (%s): %s\n", Miracle::timestamp::now().to_string_format().c_str(), msg);
    if (++count == 20) {
        g_loop->quit();
    }
}

int main()
{
    print_tid();
    Miracle::event_loop loop;
    g_loop = &loop;

    print("main");
    loop.run_after(5, std::bind(print, "after 5, once"));
    loop.run_after(2, std::bind(print, "after 2, once"));
    loop.run_every(2, std::bind(print, "every 2"));
    
    loop.loop();
    print("main loop exits");
    sleep(1);
}

