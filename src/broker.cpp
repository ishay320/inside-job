#include "broker.h"

#include <assert.h>

#include <chrono>
#include <iostream>
#include <string.h>
#include <thread>

#define UNUSED(x) (void)x

namespace insideJob
{
Broker::Broker()
{
    sem_init(&_work, 0, 0);
}

Broker::~Broker()
{
    if (_thread.joinable())
    {
        stop();
    }

    sem_destroy(&_work);
}

handle Broker::connect()
{
    static handle id = 0;
    return id++;
}

bool Broker::publish(const std::string& topic, void* data, size_t len)
{
    if (isQueueFull())
    {
        std::cout << "WARNING: broker buffer is full\n";
        return false;
    }

    std::vector<std::string> parsed_topic = parseTopic(topic);

    void* data_ptr = malloc(len);
    memcpy(data_ptr, data, len);
    std::shared_ptr<void> data_share(data_ptr, free);

    if (!_buffer.push({parsed_topic, data_share, len}))
    {
        std::cout << " WARNING: buffer is full\n";
        return false;
    }
    sem_post(&_work);
    return true;
}

void Broker::subscribe(const std::string& topic, const handle& hand, Callback callback)
{
    std::vector<std::string> parsed_topic;
    parsed_topic = parseTopic(topic);

    std::lock_guard lock{_tree_mutex};
    _tree.insert(parsed_topic.data(), parsed_topic.size(),
                 std::pair<handle, Callback>{hand, callback});
}

bool Broker::removeSub(const std::string& topic, const handle& hand)
{
    if (topic.empty())
    {
        std::cout << "ERROR: topic is empty, cannot unsubscribe\n";
        return false;
    }

    std::vector<std::string> parsed_topic;
    parsed_topic = parseTopic(topic);

    auto cmp_fun = [](std::pair<handle, Callback> a, std::pair<handle, Callback> b) -> bool
    { return a.first == b.first; };

    std::lock_guard lock{_tree_mutex};
    return _tree.remove(parsed_topic.data(), parsed_topic.size(),
                        std::pair<handle, Callback>{hand, nullptr}, cmp_fun);
}

void Broker::start()
{
    _thread = std::thread{entry, this};
}

void Broker::stop()
{
    _should_run = false;
    sem_post(&_work);
    _thread.join();

    // TODO: wait for the loop to stop using semaphore and delete all
}

void Broker::printBuffer() // FIXME: I`m broken
{
    const auto [topic, s, l] = _buffer.peak();
    for (auto&& entry : topic)
    {
        std::cout << entry << '/';
    }
    std::cout << " num: " << l << '\n';
}

void Broker::printTree()
{
    std::lock_guard lock{_tree_mutex};
    _tree.printTopics([](std::string a) { std::cout << a << " "; });
    std::cout << std::endl;
}

void Broker::entry(Broker* that)
{
    that->run();
}

void Broker::run()
{
    while (_should_run)
    {
        // sleep if buffer empty
        if (queueSize() == 0)
        {
            sem_wait(&_work);
            continue;
        }

        // pull from buffer
        const auto [topic, data, len] = std::move(_buffer.pop());

        // get clients from tree
        _tree_mutex.lock();
        std::vector<std::pair<handle, Callback>> clients =
            _tree.getPath(topic.data(), topic.size());
        _tree_mutex.unlock();

        // push
        for (auto&& client : clients)
        {
            client.second(data, len);
        }
    }
}

// TODO: better check with more elegant algo and trimming
std::vector<std::string> Broker::parseTopic(const std::string& topic)
{
    constexpr char delimiter = '/';
    std::vector<std::string> parsed_topic;
    if (topic.empty() || topic == "/")
    {
        return {};
    }

    size_t last = 0;
    size_t next = 0;
    while ((next = topic.find(delimiter, last)) != std::string::npos)
    {
        if (next - last == 0)
        {
            last = next + 1;
            continue;
        }

        parsed_topic.push_back(topic.substr(last, next - last));
        last = next + 1;
    }
    parsed_topic.push_back(topic.substr(last));
    return parsed_topic;
}

bool Broker::isQueueFull()
{
    // with this approach im losing one space, another option is to use bool flag
    return queueSize() == (BROKER_QUEUE_SIZE - 1);
}

size_t Broker::queueSize()
{
    return _buffer.size();
}

} // namespace insideJob
