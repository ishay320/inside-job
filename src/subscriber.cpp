#include "subscriber.h"

namespace insideJob
{

Subscriber::Subscriber(Broker& broker) : _broker(broker)
{
    _hand = broker.connect();
}
Subscriber::~Subscriber()
{
    // TODO: remove from the broker then delete queue
}

void Subscriber::subscribe(const std::string& topic)
{
    Callback callback =
        std::bind(&Subscriber::pushData, this, std::placeholders::_1, std::placeholders::_2);
    _broker.subscribe(topic, _hand, callback);
}

bool Subscriber::queueEmpty() const
{
    return _queue.empty();
}

size_t Subscriber::queueSize() const
{
    return _queue.size();
}

std::pair<std::shared_ptr<void>, size_t> Subscriber::peakData()
{
    return _queue.peak();
}

std::pair<std::shared_ptr<void>, size_t> Subscriber::popData()
{
    return _queue.pop();
}

bool Subscriber::pushData(std::shared_ptr<void> data, size_t len)
{
    return _queue.push(std::pair{data, len});
}

} // namespace insideJob
