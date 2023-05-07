#include <chrono>
#include <thread>

#include "broker.h"
#include "publisher.h"
#include "subscriber.h"

#include "tests.h"

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

    using namespace std::chrono_literals;
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
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20ms);
    return true;
}

/* Tests Running */
Test tests[] = {
    {test_routing, "test_routing"},
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
