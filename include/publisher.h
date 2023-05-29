#pragma once

#include "broker.h"

namespace insideJob
{
class Publisher
{
public:
    Publisher(Broker& broker);
    ~Publisher();

    void publish(const std::string& topic, void* data, size_t len);

private:
    Broker& _broker;
};

} // namespace insideJob