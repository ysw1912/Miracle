/*************************************************************************
    > File Name: self_deadlock.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-24
*************************************************************************/

#include "../mutex.h"
#include "../thread.h"

#include <iostream>
#include <set>

using namespace std;
using Miracle::lock;
using Miracle::lock_guard;

class Request;

class Inventory
{
public:
    void add(Request* req)
    {
        lock_guard lg(m_mutex);
        m_requests.insert(req);
    }

    void remove(Request* req)
    {
        lock_guard lg(m_mutex);
        m_requests.erase(req);
    }

    void printall() const; 

private:
    mutable lock m_mutex;
    set<Request*> m_requests;
};

Inventory g_inventory;

class Request 
{
public:
    void process()
    {
        lock_guard lg(m_mutex);
        g_inventory.add(this);
    }

    ~Request()
    {
        lock_guard lg(m_mutex);
        sleep(1);
        g_inventory.remove(this);
    }

    void print() const 
    {
        lock_guard lg(m_mutex);
        printf("Request::print()\n");
    }

private:
    mutable lock m_mutex;
};

void Inventory::printall() const 
{
    lock_guard lg(m_mutex);
    sleep(1);
    for (set<Request*>::const_iterator it = m_requests.begin();
         it != m_requests.end();
         ++it) {
        (*it)->print();
    }
    printf("Inventory::printall() unlocked\n");
}

void thread_func()
{
    Request* req = new Request;
    req->process();
    delete req;
}

int main()
{
    Miracle::thread thr(thread_func);
    thr.start();
    ::usleep(500 * 1000);   // 0.5s
    g_inventory.printall();
    thr.join();
    return 0;
}
