#pragma once

#include "broker.h"

#include <functional>
#include <iostream>
#include <queue>

namespace insideJob
{

class Subscriber
{
public:
    Subscriber(Broker& broker) : _broker(broker)
    {
        _hand  = broker.connect();
        _queue = new std::queue<std::pair<std::shared_ptr<void>, size_t>>;
    }
    ~Subscriber()
    {
        while (!_queue->empty())
        {
            _queue->pop();
        }

        // TODO: remove from the broker then delete queue
    }

    void subscribe(const std::string& topic)
    {

        Callback callback =
            std::bind(&Subscriber::pushData, this, std::placeholders::_1, std::placeholders::_2);
        _broker.subscribe(topic, _hand, callback);
    }

    bool queueEmpty() const { return _queue->empty(); }

    std::pair<std::shared_ptr<void>, size_t> popData()
    {
        // TODO: lock queue?
        auto ret = _queue->front();
        _queue->pop();
        return ret;
    }

private:
    bool pushData(std::shared_ptr<void> data, size_t len)
    {
        _queue->push(std::pair{data, len});
        return true;
    }

    Broker& _broker;
    handle _hand;
    std::queue<std::pair<std::shared_ptr<void>, size_t>>* _queue;
};

} // namespace insideJob