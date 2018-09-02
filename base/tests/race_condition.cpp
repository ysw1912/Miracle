/*************************************************************************
    > File Name: race_condition.cpp
    > Author: Stewie
    > E-mail: 793377164@qq.com
    > Created Time: 2018-08-22
*************************************************************************/

#include "../thread.h"
#include "../mutex.h"

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

using std::string;
using Miracle::lock;
using Miracle::lock_guard;

void sleep_ms(int ms)
{
    ::usleep(ms * 1000);
}

class Stock : boost::noncopyable 
{
public:
    Stock(const string& name) : m_name(name)
    {
        printf("%s:\tStock()[%p] %s\n", Miracle::this_thread::name(), this, m_name.c_str());
    }

    ~Stock()
    {
        printf("%s:\t~Stock()[%p] %s\n", Miracle::this_thread::name(), this, m_name.c_str());
    }

    const string& key() const 
    { return m_name; }

private:
    string m_name;
};

class StockFactory : boost::noncopyable 
{
public:
    std::shared_ptr<Stock>
    get(const string& key)
    {
        std::shared_ptr<Stock> pStock;
        lock_guard lg(m_lock);
        std::weak_ptr<Stock>& wkStock = m_stocks[key];
        pStock = wkStock.lock();
        if (!pStock) {
            pStock.reset(new Stock(key),
                         [this](Stock* stock) { delete_stock(stock); });
            wkStock = pStock;
        }
        return pStock;
    }

private:
    void delete_stock(Stock* stock)
    {
        printf("%s:\tdelete_stock[%p]\n", Miracle::this_thread::name(), stock);
        if (stock) {
            sleep_ms(500);
            lock_guard lg(m_lock);
            auto it = m_stocks.find(stock->key());
            assert(it != m_stocks.end());
            if (it->second.expired()) {
                m_stocks.erase(it);
            }
        }
        delete stock;
    }

private:
    mutable lock m_lock;
    std::unordered_map<string, std::weak_ptr<Stock> > m_stocks;
};

void threadB(StockFactory* factory)
{
    sleep_ms(250);
    auto stock1 = factory->get("MS");
    printf("%s:\tget stock1 %p\n", Miracle::this_thread::name(), stock1.get());

    sleep_ms(500);
    auto stock2 = factory->get("MS");
    printf("%s:\tget stock2 %p\n", Miracle::this_thread::name(), stock2.get());
    
    if (stock1 != stock2) {
        printf("FBI WARNING: stock1 != stock2\n");
    }
}

int main()
{
    StockFactory factory;
    Miracle::thread thr([&factory] { threadB(&factory); }, "thread B");
    thr.start();

    {
        auto stock = factory.get("MS");
        printf("%s:\tget stock %p\n", Miracle::this_thread::name(), stock.get());
    }

    thr.join();
    return 0;
}
