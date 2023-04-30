#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "broker.h"
#include "node.h"
#define UNUSED(x) (void)x

void printByLen(void* data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        printf("%c", ((char*)data)[i]);
    }
}

// TODO: node tests

/**
 * @brief check if the broker can run and stop on command
 */
bool test_runAndStop()
{
    insideJob::Broker broker;
    broker.start();
    // check that its running
    insideJob::handle hand = broker.connect();
    bool received          = false;

    broker.subscribe("/", hand,
                     [&received](const void*, size_t) -> bool
                     {
                         received = true;
                         return true;
                     });
    broker.publish("/", NULL, 0);
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20ms);
    if (!received)
    {
        std::cout << "broker did not started\n";
        return false;
    }

    broker.stop();
    // check that it stopped
    received = false;
    broker.publish("/", NULL, 0);
    std::this_thread::sleep_for(20ms);
    if (received)
    {
        std::cout << "broker did not stopped\n";
        return false;
    }
    return true;
}

/**
 * @brief this test tests the limit of the broker queue
 */
bool test_brokerLimit()
{
    bool err;
    insideJob::Broker broker;
    for (size_t i = 0; i < BROKER_QUEUE_SIZE - 1; i++)
    {
        err = broker.publish("", NULL, 0);
        if (!err)
        {
            std::cout << "failed in normal publish\n";
            return false;
        }
    }
    err = broker.publish("", NULL, 0);
    if (err)
    {
        std::cout << "failed in overflow publish\n";
        return false;
    }
    return true;
}

typedef bool (*TestFun)();
typedef struct
{
    TestFun test;
    const char* name;
} Test;

int main(int argc, char const* argv[])
{
    UNUSED(argv);
    UNUSED(argc);

    Test tests[] = {{test_brokerLimit, "test_brokerLimit"}, {test_runAndStop, "test_runAndStop"}};

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
    {
        if (!tests[i].test())
        {
            std::cout << "\033[31m"
                      << "[-] Test `" << tests[i].name << "` failed"
                      << " \033[0m"
                      << "\n";
        }
        else
        {
            std::cout << "\033[32m"
                      << "[+] Test `" << tests[i].name << "` pass"
                      << " \033[0m"
                      << "\n";
        }
    }

    return 0;
}
