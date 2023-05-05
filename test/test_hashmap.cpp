#include "broker.h"
#include "hashmap_tree.h"
#include "tests.h"

/* Globals */
using hashTree     = HashmapTree<std::string, std::pair<insideJob::handle, std::string>>;
const auto cmp_fun = [](std::pair<insideJob::handle, std::string> a,
                        std::pair<insideJob::handle, std::string> b) -> bool
{ return a.first == b.first; };

/* Helpers functions */
static bool _checkRemoved(hashTree& hash, std::vector<std::string>& topic,
                          insideJob::handle expected_count);

/**
 * @brief check that insert in the tree in the right order and get it the same
 */
bool test_subTopics()
{
    hashTree hash;

    std::vector<std::string> topic1     = {"hello", "world"};
    std::vector<std::string> topic1_1   = {"hello"};
    std::vector<std::string> topic2     = {"world"};
    std::vector<std::string> topic_root = {};
    hash.insert(topic1_1.data(), topic1_1.size(), {2, "haius"});
    hash.insert(topic1.data(), topic1.size(), {1, "hi mom"});
    hash.insert(topic2.data(), topic2.size(), {2, "hello mom"});
    hash.insert(topic2.data(), topic2.size(), {2, "hello mom"});

    {
        std::vector<std::pair<insideJob::handle, std::string>> root =
            hash.get(topic_root.data(), topic_root.size());
        if (root.size() != 4)
        {
            LOG_ERROR("root wont get all the subtopics");
            return 1;
        }
    }
    {
        std::vector<std::pair<insideJob::handle, std::string>> hw =
            hash.get(topic1.data(), topic1.size());
        if (hw.size() != 1)
        {
            LOG_ERROR("topic1 not correctly get");
            return 1;
        }
    }
    {
        std::vector<std::pair<insideJob::handle, std::string>> hw =
            hash.get(topic1_1.data(), topic1_1.size());
        if (hw.size() != 2)
        {
            LOG_ERROR("topic1_1 not correctly get");
            return 1;
        }
    }
    {
        std::vector<std::pair<insideJob::handle, std::string>> w =
            hash.get(topic2.data(), topic2.size());
        if (w.size() != 2)
        {
            LOG_ERROR("topic2 not correctly get");
            return 1;
        }
    }

    return true;
}

/**
 * @brief check that insert get and remove working correctly
 */
bool test_addAndRemove()
{
    hashTree hash;

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
        if (!hash.remove(topic.data(), topic.size(), {2, ""}, cmp_fun))
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
        if (!hash.remove(topic.data(), topic.size(), {1, ""}, cmp_fun))
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

/* Tests Running */
Test tests[] = {
    {test_addAndRemove, "test_addAndRemove"},
    {test_subTopics, "test_subTopics"},
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

static bool _checkRemoved(hashTree& hash, std::vector<std::string>& topic,
                          insideJob::handle expected_count)
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
