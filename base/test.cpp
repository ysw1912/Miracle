/*************************************************************************
    > File Name: test.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-20
*************************************************************************/

#include "atomic.h"

#include <iostream>


using namespace std;

using Miracle::atomic_int32_t; 

int main()
{
    atomic_int32_t a;
    printf("a = %d\n", a.get());
    a.increment();
    printf("a = %d\n", a.increment_and_get());

    return 0;
}
