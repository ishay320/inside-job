#pragma once

#include "broker.h"
#include "circular_queue.h"

#include <functional>

#define SUBSCRIBER_QUEUE_SIZE 128

namespace insideJob
{

class Subscriber
{
public:
    Subscriber(Broker& broker) : _broker(broker) { _hand = broker.connect(); }
    ~Subscriber()
    {
        // TODO: remove from the broker then delete queue
    }

    void subscribe(const std::string& topic)
    {

        Callback callback =
            std::bind(&Subscriber::pushData, this, std::placeholders::_1, std::placeholders::_2);
        _broker.subscribe(topic, _hand, callback);
    }

    bool queueEmpty() const { return _queue.empty(); }

    size_t queueSize() const { return _queue.size(); }

    std::pair<std::shared_ptr<void>, size_t> popData() { return _queue.pop(); }

private:
    bool pushData(std::shared_ptr<void> data, size_t len)
    {
        return _queue.push(std::pair{data, len});
    }

    Broker& _broker;
    handle _hand;
    insideJob::CircularQueue<std::pair<std::shared_ptr<void>, size_t>, SUBSCRIBER_QUEUE_SIZE>
        _queue;
};

} // namespace insideJob