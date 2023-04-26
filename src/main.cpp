#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "broker.h"
#define UNUSED(x) (void)x

bool foo(void* data, size_t len)
{
    UNUSED(data);
    UNUSED(len);
    std::cout << "fooooooya\n";
    return true;
}

int main(int argc, char const* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    insideJob::Broker broker;

    insideJob::handle c = broker.connect();

    broker.subscribe("main/test", c, foo);
    broker.subscribe("main/test2", c,
                     [](void* data, size_t len) -> bool
                     {
                         UNUSED(data);
                         UNUSED(len);
                         std::cout << "im in!\n";
                         return true;
                     });

    broker.publish("main/test", NULL, 0);
    broker.publish("main/test2", NULL, 0);
    broker.publish("main/test2", NULL, 0);
    broker.publish("main/test2", NULL, 0);

    // should fail
    broker.publish("main/test2", NULL, 0);
    broker.publish("main/test2", NULL, 0);

    broker.printBuffer();
    broker.start();

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);

    std::cout << "publish\n";
    broker.publish("main/test2", NULL, 0);
    broker.publish("main/test2", NULL, 0);
    std::this_thread::sleep_for(1s);

    broker.publish("main/test2", NULL, 0);
    broker.publish("main/test2", NULL, 0);
    std::this_thread::sleep_for(5s);
    return 0;
}
