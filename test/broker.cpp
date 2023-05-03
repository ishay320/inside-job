#include "broker.h"
#include "tests.h"

#include <chrono>
#include <thread>

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
        LOG_ERROR("broker did not started");
        return false;
    }

    broker.stop();
    // check that it stopped
    received = false;
    broker.publish("/", NULL, 0);
    std::this_thread::sleep_for(20ms);
    if (received)
    {
        LOG_ERROR("broker did not stopped");
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
            LOG_ERROR("failed in normal publish");
            return false;
        }
    }
    err = broker.publish("", NULL, 0);
    if (err)
    {
        LOG_ERROR("failed in overflow publish");
        return false;
    }
    return true;
}

Test tests[] = {
    {test_brokerLimit, "test_brokerLimit"},
    {test_runAndStop, "test_runAndStop"},
};
const size_t tests_len = sizeof(tests) / sizeof(tests[0]);

int main(void)
{
    if (run_tests(tests, tests_len, "main"))
    {
        LOG_ERROR("main tests failed");
        return 1;
    }

    return 0;
}
