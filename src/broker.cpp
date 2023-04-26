#include "broker.h"

#include <assert.h>

#include <chrono>
#include <iostream>
#include <thread>

#define UNUSED(x) (void)x

namespace insideJob
{
Broker::Broker() {}
Broker::~Broker() {}

handle Broker::connect()
{
    static handle id = 0;
    return id++;
}

bool Broker::publish(const std::string& topic, void* data, size_t len)
{
    int queue_pos = getQueueHead();
    if (queue_pos == -1)
    {
        std::cout << "WARNING: broker buffer is full\n";
        return false;
    }

    _buffer[queue_pos] = std::make_tuple(topic, data, len);
    return true;
}

void Broker::subscribe(const std::string& topic, const handle& hand, Callback callback)
{
    if (topic.empty())
    {
        std::cout << "ERROR: topic is empty, cannot subscribe\n";
        return;
    }

    std::vector<std::string> parsed_topic;
    parsed_topic = parseTopic(topic);

    _tree.insert(parsed_topic.data(), parsed_topic.size(),
                 std::pair<handle, Callback>{hand, callback});
}
void Broker::remove_sub(const std::string& topic, const handle& hand)
{
    UNUSED(topic);
    UNUSED(hand);
    assert(false && "function not implemented yet");
}

void Broker::start()
{
    std::thread thread(entry, this);
    thread.detach();
}

void Broker::stop()
{
    _should_run = false;
}

void Broker::printBuffer()
{
    for (size_t i = _queue_tail; i != _queue_head; i = (i + 1) % BROKER_QUEUE_SIZE)
    {
        const auto [f, s, l] = _buffer[i];
        std::cout << "pos: " << i << " data: " << f << " num: " << l << '\n';
    }
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
            // TODO: replace with semaphore
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ms);
            continue;
        }

        // pull from buffer
        const auto [str, data, len] = _buffer[_queue_tail];

        // release queue space
        _queue_tail = (_queue_tail + 1) % BROKER_QUEUE_SIZE;

        // parse
        auto topic = parseTopic(str);

        // get clients from tree
        std::vector<std::pair<handle, Callback>> clients = _tree.get(topic.data(), topic.size());

        // push
        for (auto&& client : clients)
        {
            client.second(data, len);
        }
    }
}

std::vector<std::string> Broker::parseTopic(const std::string& topic)
{
    constexpr char delimiter = '/';
    std::vector<std::string> parsed_topic;

    size_t last = 0;
    size_t next = 0;
    while ((next = topic.find(delimiter, last)) != std::string::npos)
    {
        parsed_topic.push_back(topic.substr(last, next - last));
        last = next + 1;
    }
    parsed_topic.push_back(topic.substr(last));
    return parsed_topic;
}

int Broker::getQueueHead()
{
    if (isQueueFull())
    {
        return -1;
    }
    size_t ret  = _queue_head;
    _queue_head = (_queue_head + 1) % BROKER_QUEUE_SIZE;
    return ret;
}

bool Broker::isQueueFull()
{
    // with this approach im losing one space, another option is to use bool flag
    return queueSize() == (BROKER_QUEUE_SIZE - 1);
}

size_t Broker::queueSize()
{
    size_t queue_size = 0;
    if (_queue_head < _queue_tail)
    {
        queue_size = _queue_head + BROKER_QUEUE_SIZE - _queue_tail;
    }
    else
    {
        queue_size = _queue_head - _queue_tail;
    }
    return queue_size;
}

} // namespace insideJob
