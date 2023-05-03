#include "broker.h"
#include "hashmap_tree.h"
#include "tests.h"

/** @brief helper function */
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

Test tests[]           = {{test_hashMapTree, "test_hashMapTree"}};
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