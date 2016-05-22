#include "echo_server.hpp"
#include <iostream>

using namespace std::chrono;

// this could be some long running computation or I / O
std::future<int> calculate_the_answer()
{
    return std::async([] {
        std::this_thread::sleep_for(1s); return 42;
    });
}

// Here is a resumable function
std::future<void> coro() 
{
    std::cout << "Started waiting.." << std::endl;
    auto result = __await calculate_the_answer();
    printf("got:%d" , result);
    //std::cout << "got:" << result << std::endl;
}
int main()
{
    //coro().get();
    io_service io_service;
    Server server(io_service);
    server.start_accept();
    io_service.run();
    return 0;
}