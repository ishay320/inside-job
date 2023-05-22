#include <chrono>
#include <string.h>
#include <thread>

#include "broker.h"
#include "publisher.h"
#include "subscriber.h"

#include "tests.h"

using namespace std::chrono_literals;

bool test_stress()
{

    typedef struct
    {
        int num;
        char str[64];
    } Data;

    insideJob::Broker broker;
    broker.start();

    insideJob::Subscriber sub{broker};
    insideJob::Publisher pub{broker};

    std::string topic{"/base/some1"};
    sub.subscribe(topic);

    for (size_t i = 0; i < 100; i++)
    {

        Data packet{.num = (int)i, .str = "test1"};
        pub.publish(topic, (void*)&packet, 64);
        Data packet2{.num = -(int)i, .str = "test2"};
        pub.publish(topic, (void*)&packet2, 64);

        while (sub.queueEmpty())
        {
            std::this_thread::sleep_for(1ms);
        }
        while (!sub.queueEmpty())
        {
            auto data = sub.popData();
            if (strcmp(((Data*)data.first.get())->str, "test1") == 0) // fail segfault
            {
                if (((Data*)data.first.get())->num != (int)i)
                {
                    return false;
                }
            }
            else if (strcmp(((Data*)data.first.get())->str, "test2") == 0)
            {
                if (((Data*)data.first.get())->num != -(int)i)
                {
                    return false;
                }
            }
            else
            {
                std::cout << "\033[33m" << __FILE__ << ':' << __LINE__ << " " << __FUNCTION__
                          << " \033[0m" << ((Data*)data.first.get())->str << " "
                          << ((Data*)data.first.get())->num << "\n";
            }

            // Check that the ptr is uniq
            long uniq_num = data.first.use_count();
            if (uniq_num != 1)
            {
                LOG_ERROR("ptr '" << ((Data*)data.first.get())->str
                                  << "' is not uniq: " << uniq_num);
                return false;
            }
        }
    }

    broker.stop();
    return true;
}

bool test_routing()
{
    insideJob::Broker broker;
    broker.start();
    insideJob::Subscriber sub1{broker};
    sub1.subscribe("");
    insideJob::Subscriber sub2{broker};
    sub2.subscribe("/hello");
    insideJob::Subscriber sub3{broker};
    sub3.subscribe("/hello/world");
    insideJob::Publisher pub{broker};
    pub.publish("/hello", NULL, 0);

    std::this_thread::sleep_for(20ms);

    if (!sub3.queueEmpty())
    {
        LOG_ERROR("sub3 - child of the specific sub - received topic he do not suppose to");
        return false;
    }
    if (sub2.queueEmpty())
    {
        LOG_ERROR("sub2 - the specific sub - did not received anything");
        return false;
    }
    if (sub1.queueEmpty())
    {
        LOG_ERROR("sub1 - root - did not received anything");
        return false;
    }

    broker.stop();
    return true;
}

/* Tests Running */
Test tests[] = {
    {test_routing, "test_routing"},
    {test_stress, "test_stress"},
};
const size_t tests_len = sizeof(tests) / sizeof(tests[0]);

int main(void)
{
    try
    {
        if (run_tests(tests, tests_len, "hashmap tree"))
        {
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
