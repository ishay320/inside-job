#include "broker.h"
#include "tests.h"

#include <atomic>
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
    std::atomic<bool> received;

    broker.subscribe("/", hand,
                     [&received](const std::shared_ptr<void>&, size_t) -> bool
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

    received = false;
    broker.stop();
    // check that it stopped
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
 * @brief check if the broker can parse the empty topic
 */
bool test_emptyTopic()
{
    insideJob::Broker broker;
    broker.start();
    // check that its running
    insideJob::handle hand1     = broker.connect();
    std::atomic<bool> received1 = false;
    insideJob::handle hand2     = broker.connect();
    std::atomic<bool> received2 = false;

    broker.subscribe("/", hand1,
                     [&received1](const std::shared_ptr<void>&, size_t) -> bool
                     {
                         received1 = true;
                         return true;
                     });
    broker.subscribe("", hand2,
                     [&received2](const std::shared_ptr<void>&, size_t) -> bool
                     {
                         received2 = true;
                         return true;
                     });
    broker.publish("/", NULL, 0);
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20ms);
    if (!received1)
    {
        LOG_ERROR("topic '/' not pares correctly when pushed '/'");
        return false;
    }
    if (!received2)
    {
        LOG_ERROR("topic '' not pares correctly when pushed '/'");
        return false;
    }

    // Reset
    received1 = false;
    received2 = false;

    broker.publish("", NULL, 0);
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20ms);
    if (!received1)
    {
        LOG_ERROR("topic '/' not pares correctly when pushed ''");
        return false;
    }
    if (!received2)
    {
        LOG_ERROR("topic '' not pares correctly when pushed ''");
        return false;
    }

    broker.stop();
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

/* Tests Running */

Test tests[] = {
    {test_brokerLimit, "test_brokerLimit"},
    {test_runAndStop, "test_runAndStop"},
    {test_emptyTopic, "test_emptyTopic"},
};
const size_t tests_len = sizeof(tests) / sizeof(tests[0]);

int main(void)
{
    try
    {
        if (run_tests(tests, tests_len, "broker"))
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
