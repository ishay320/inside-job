# Inside Job

"Inside Job" is a package that manages the transfer of messages within a single process between threads using pointers and callbacks, enabling efficient and lightweight communication between the threads. It utilizes an MQTT-like approach, utilizing publish/subscribe patterns to transfer data through callbacks.

## Installation - wip not installing yet

To install the Inside Job package, simply clone the repository and build the project using your preferred build system.

```sh
$ git clone https://github.com/ishay320/inside-job
$ cd inside-job
$ mkdir build
$ cmake ..
$ sudo make install
```

The default queue of broker is `4096` but can be change in compilation using `$ cmake -DBROKER_QUEUE_SIZE=<queue-size> ..`

## Usage

The Inside Job package provides a simple API to use for message transfer between threads. The main component of the API is the Broker class.

## Broker

The Broker class is used to manage connections and message transfer. To create a Broker object, simply call the constructor.

```cpp
#include "broker.h"

Broker broker();
```

The broker will be pass to all threads by pointer or reference.

## Subscribe

To connect to the Broker for subscribing, call the `connect` function, which returns a Handle object to be used for subsequent operations.

```cpp
Handle hand = broker.connect();
```

To subscribe to a topic, call the `subscribe` function with the topic name, the Handle object, and a callback function to be called when a message is received.

```cpp
bool my_callback(void* data, size_t len) {
    std::cout << "Received message: " << std::string((char*)data, len) << std::endl;
    return true;
}

broker.subscribe("my_topic", hand, my_callback);
// or
broker.subscribe("my_topic", hand,
                [](void *data, size_t len) -> bool
                {
                    std::cout << "Received message: " << std::string((char*)data, len) << std::endl;
                    return true;
                });
```

The callback function will be called with a pointer to the received data and the size of the data. It should return true if the message was successfully processed, and false otherwise.

> It is important to note that the callback function should be as lightweight as possible, as it will be called from the thread that handles message transfer.

## Publish

To publish a message to a topic, call the `publish` function with the topic name, a pointer to the data, and the size of the data.

```cpp
char data[] = "Hello, world!";
broker.publish("my_topic", data, sizeof(data));
```
