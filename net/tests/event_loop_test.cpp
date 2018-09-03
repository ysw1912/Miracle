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

Miracle::event_loop* g_loop;

void print()
{
    printf("call print()\n");
    sleep(20);
    g_loop->quit();
}

void fun()
{
    printf("hello\n");
}

void thr_fun()
{
    Miracle::event_loop loop;
    g_loop = &loop;
    printf("thr_fun() at %s\n", Miracle::timestamp::now().to_string().c_str());
    g_loop->run_after(3, print);
    usleep(300);
    g_loop->loop();
}

int main()
{
    Miracle::thread thr(thr_fun);
    thr.start();

    usleep(100);
    g_loop->run_every(1, fun);

    thr.join();
}

