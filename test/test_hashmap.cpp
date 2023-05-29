#include "broker.h"
#include "hashmap_tree.h"
#include "tests.h"

/* Globals */
using hashTree     = insideJob::HashmapTree<std::string, std::pair<insideJob::handle, std::string>>;
const auto cmp_fun = [](std::pair<insideJob::handle, std::string> a,
                        std::pair<insideJob::handle, std::string> b) -> bool
{ return a.first == b.first; };

/* Helpers functions */
static bool _checkRemoved(hashTree& hash, std::vector<std::string>& topic,
                          insideJob::handle expected_count);
static bool _checkGets(hashTree& hash, const std::vector<std::string>& topic, size_t expected_down,
                       const std::string& err_down, size_t expected_up, const std::string& err_up);

bool test_gets()
{
    hashTree hash;

    std::vector<std::string> topic_root = {};
    std::vector<std::string> topic1     = {"hello"};
    std::vector<std::string> topic1_1   = {"hello", "world"};
    std::vector<std::string> topic1_2   = {"hello", "mama"};
    std::vector<std::string> topic2     = {"world"};

    // Check empty tree
    if (!_checkGets(hash, topic_root, 0, "get down should return empty when tree is empty", 0,
                    "get up should return empty when tree is empty"))
    {
        return false;
    }

    // Check get the root
    hash.insert(topic_root.data(), topic_root.size(), {2, "haius"});
    if (!_checkGets(hash, topic_root, 1, "get down wont gets the root topic", 1,
                    "get up wont gets the root topic"))
    {
        return false;
    }

    // Check one down
    if (!_checkGets(hash, topic1, 0, "get down should return parents", 1,
                    "get up should not return parents"))
    {
        return false;
    }

    hash.insert(topic1.data(), topic1.size(), {1, "hi mom"});
    if (!_checkGets(hash, topic1, 1, "get down gets more then it subscribed", 2,
                    "get up wont gets gets parents"))
    {
        return false;
    }

    hash.insert(topic1_1.data(), topic1_1.size(), {2, "hello mom"});
    if (!_checkGets(hash, topic1, 2, "get down gets more then it subscribed", 2,
                    "get up wont gets gets parents"))
    {
        return false;
    }

    // Check getting a topic from a non exist branch
    if (!_checkGets(hash, topic2, 0, "get down needs to be empty", 1,
                    "get up do not return parents"))
    {
        return false;
    }

    return true;
}
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
            hash.getDown(topic_root.data(), topic_root.size());
        if (root.size() != 4)
        {
            LOG_ERROR("root wont get all the subtopics");
            return 1;
        }
    }
    {
        std::vector<std::pair<insideJob::handle, std::string>> hw =
            hash.getDown(topic1.data(), topic1.size());
        if (hw.size() != 1)
        {
            LOG_ERROR("topic1 not correctly get");
            return 1;
        }
    }
    {
        std::vector<std::pair<insideJob::handle, std::string>> hw =
            hash.getDown(topic1_1.data(), topic1_1.size());
        if (hw.size() != 2)
        {
            LOG_ERROR("topic1_1 not correctly get");
            return 1;
        }
    }
    {
        std::vector<std::pair<insideJob::handle, std::string>> w =
            hash.getDown(topic2.data(), topic2.size());
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
    {test_gets, "test_gets"},
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
        hash.getDown(topic.data(), topic.size());
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

static bool _checkGets(hashTree& hash, const std::vector<std::string>& topic, size_t expected_down,
                       const std::string& err_down, size_t expected_up, const std::string& err_up)
{
    auto down = hash.getDown(topic.data(), topic.size());
    auto up   = hash.getPath(topic.data(), topic.size());

    if (down.size() != expected_down)
    {
        LOG_ERROR(err_down << ", got: " << down.size());
        return false;
    }
    if (up.size() != expected_up)
    {
        LOG_ERROR(err_up << ", got: " << up.size());
        return false;
    }
    return true;
}
