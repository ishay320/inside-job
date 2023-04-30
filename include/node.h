#include "broker.h"

#include <functional>
#include <iostream>
#include <queue>

namespace insideJob
{

class node
{
public:
    node(Broker& broker) : _broker(broker)
    {
        _hand  = broker.connect();
        _queue = new std::queue<std::pair<void*, size_t>>;
    }
    ~node()
    {
        // TODO: remove from the broker then delete queue
    }

    void publish(const std::string& topic, void* data, size_t len)
    {
        _broker.publish(topic, data, len);
    }

    void subscribe(const std::string& topic)
    {

        Callback callback =
            std::bind(&node::pushData, *this, std::placeholders::_1, std::placeholders::_2);
        _broker.subscribe(topic, _hand, callback);
    }

    bool queueEmpty() { return _queue->empty(); }

    std::pair<void*, size_t> popData()
    {
        // TODO: lock queue?
        auto ret = _queue->front();
        _queue->pop();
        return ret;
    }

private:
    bool pushData(void* data, size_t len)
    {
        _queue->push(std::pair{data, len});
        return true;
    }

    Broker& _broker;
    handle _hand;
    std::queue<std::pair<void*, size_t>>* _queue;
};

} // namespace insideJob