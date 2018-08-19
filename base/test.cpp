/*************************************************************************
    > File Name: test.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-20
*************************************************************************/

#include "timestamp.h"
#include "stdio.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

#include <iostream>

using Miracle::timestamp;

int main()
{
    timestamp ts = timestamp::now();
    printf("timestamp: %" PRId64 "\n", ts.usec_since_epoch());
    printf("to_string: %s\n", ts.to_string().c_str());
    printf("to_string_format: %s\n", ts.to_string_format().c_str());
    return 0;
}
