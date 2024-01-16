#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <future>

#define VASTINA_CPP
#include "../include/tools.h"
#include "../include/client.hpp"
#include "../include/ThreadPool.hpp"


int main(){

    vastina_log::logtest("start");

    printf("port to connect: 8888\n");
    client *client_ = new client();

    client_->init();

    client_->setclientsock(AF_INET, SOCK_STREAM, IPPROTO_TCP, -1);
    client_->connect_(AF_INET, SOCK_STREAM, IPPROTO_TCP, 8888);
    
    ThreadPool pool(2);
    std::vector< std::future<int> > results{};

    results.emplace_back(pool.enqueue([&client_]()->int{
        client_->test1();
        return 1;
    } ) );

    results.emplace_back(pool.enqueue([&client_]()->int{
        client_->test2();
        return 2;
    } ) );

    for(int i=1;auto&result: results) print("({},{})\n", i, result.get()), ++i;

    client_->end();

    return 0;
}
