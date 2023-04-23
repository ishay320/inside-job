#include <iostream>
#include <string>

#include "broker.h"

bool foo(void *data, size_t len)
{
    std::cout << "fooooooya\n";
    return true;
}

int main(int argc, char const *argv[])
{
    Broker<5> broker;

    handle c = broker.connect();

    broker.subscribe("main/test", c, foo);
    broker.subscribe("main/test2", c,
                     [](void *data, size_t len) -> bool
                     {
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
    broker.run();
    return 0;
}
