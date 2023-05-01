#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "broker.h"
#include "node.h"
#include "utils.h"
#define UNUSED(x) (void)x
// TODO: node tests

void printByLen(void* data, size_t len);
static bool _checkRemoved(HashmapTree<std::string, std::pair<insideJob::handle, std::string>>& hash,
                          std::vector<std::string>& topic, insideJob::handle expected_count);

/**
 * @brief check that insert get and remove working correctly
 */
bool test_hashMapTree()
{
    HashmapTree<std::string, std::pair<insideJob::handle, std::string>> hash;

    std::vector<std::string> topic = {"hello", "world"};
    hash.insert(topic.data(), topic.size(), {1, "hi mom"});
    hash.insert(topic.data(), topic.size(), {2, "hello mom"});

    // Check if all the data received correctly
    if (!_checkRemoved(hash, topic, 2))
    {
        return false;
    }

    // Remove num 2
    {
        bool ret = hash.remove(topic.data(), topic.size(), {2, ""},
                               [](std::pair<insideJob::handle, std::string> a,
                                  std::pair<insideJob::handle, std::string> b) -> bool
                               { return a.first == b.first; });
        if (!ret)
        {
            LOG_ERROR("failed at removing num 2");
            return false;
        }
    }

    // Check that it removed
    if (!_checkRemoved(hash, topic, 1))
    {
        return false;
    }

    // Remove num 1
    {
        bool ret = hash.remove(topic.data(), topic.size(), {1, ""},
                               [](std::pair<insideJob::handle, std::string> a,
                                  std::pair<insideJob::handle, std::string> b) -> bool
                               { return a.first == b.first; });
        if (!ret)
        {
            LOG_ERROR("failed at removing num 1");
            return false;
        }
    }

    // Check that the topic does not exist
    if (!_checkRemoved(hash, topic, 0))
    {
        return false;
    }
    return true;
}

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

    Test tests[] = {{test_brokerLimit, "test_brokerLimit"},
                    {test_runAndStop, "test_runAndStop"},
                    {test_hashMapTree, "test_hashMapTree"}};

    const size_t test_len = sizeof(tests) / sizeof(tests[0]);
    for (size_t i = 0; i < test_len; i++)
    {
        if (!tests[i].test())
        {
            std::cout << "\033[31m"
                      << "[-] Test " << i + 1 << '/' << test_len << " '" << tests[i].name
                      << "' failed "
                      << " \033[0m"
                      << "\n";
        }
        else
        {
            std::cout << "\033[32m"
                      << "[+] Test " << i + 1 << '/' << test_len << " '" << tests[i].name
                      << "' pass "
                      << " \033[0m"
                      << "\n";
        }
    }

    return 0;
}

void printByLen(void* data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        printf("%c", ((char*)data)[i]);
    }
}

static bool _checkRemoved(HashmapTree<std::string, std::pair<insideJob::handle, std::string>>& hash,
                          std::vector<std::string>& topic, insideJob::handle expected_count)
{
    std::vector<std::pair<insideJob::handle, std::string>> hw =
        hash.get(topic.data(), topic.size());
    insideJob::handle count = 0;
    for (auto&& i : hw)
    {
        if (++count != i.first)
        {
            LOG_ERROR("failed at receiving " << count << "received " << i.first);
            return false;
        }
    }
    if (count != expected_count)
    {
        LOG_ERROR("failed at receiving all the data, received " << count << " of "
                                                                << expected_count);
        return false;
    }
    return true;
}