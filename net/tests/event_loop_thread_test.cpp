/*************************************************************************
    > File Name: event_loop_thread_test.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-09-24
*************************************************************************/

#include "../event_loop.h"
#include "../event_loop_thread.h"

#include <stdio.h>

void run()
{
    printf("run():  pid = %d, tid = %d\n", getpid(), Miracle::this_thread::id());
}

int main()
{
    printf("main(): pid = %d, tid = %d\n", getpid(), Miracle::this_thread::id());

    Miracle::event_loop_thread loop_thread;

    Miracle::event_loop* loop = loop_thread.start();
    loop->run_in_loop(run);

    sleep(1);
    loop->run_after(2, run);

    sleep(3);
    loop->quit();

    printf("main(): exit\n");

    return 0;
}
