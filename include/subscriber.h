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
    Subscriber(Broker& broker);
    ~Subscriber();

    void subscribe(const std::string& topic);

    bool queueEmpty() const;
    size_t queueSize() const;

    std::pair<std::shared_ptr<void>, size_t> peakData();
    std::pair<std::shared_ptr<void>, size_t> popData();

protected:
    bool pushData(std::shared_ptr<void> data, size_t len);

private:
    Broker& _broker;
    handle _hand;
    insideJob::CircularQueue<std::pair<std::shared_ptr<void>, size_t>, SUBSCRIBER_QUEUE_SIZE>
        _queue;
};

} // namespace insideJob