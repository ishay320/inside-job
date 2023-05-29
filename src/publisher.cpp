#include "publisher.h"

namespace insideJob
{
Publisher::Publisher(Broker& broker) : _broker(broker) {}
Publisher::~Publisher() {}

void Publisher::publish(const std::string& topic, void* data, size_t len)
{
    _broker.publish(topic, data, len);
}

} // namespace insideJob