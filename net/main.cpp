/*************************************************************************
    > File Name: main.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-09-01
*************************************************************************/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    vector<int> v{1, 2, 2, 3, 3, 4, 4, 5};
    vector<pair<int, int*>> vp(v.size());
    for (int i = 0; i < int(v.size()); ++i) {
        vp[i].first = v[i];
        vp[i].second = &v[i];
    }
    pair<int, int*> sentry;
    sentry.first = 3;
    sentry.second = reinterpret_cast<int*>(INTPTR_MAX);
    cout << lower_bound(vp.begin(), vp.end(), sentry) - vp.begin() << endl;
    cout << upper_bound(vp.begin(), vp.end(), sentry) - vp.begin() << endl;
}
