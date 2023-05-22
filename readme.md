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

The Inside Job package provides a simple API to use for message transfer between threads. The main component of the API is the Broker class. the messaging api is using the `publish` and `subscribe` classes

## Broker

The `Broker` class is responsible for managing connections and facilitating message transfer. To create a `Broker` object, simply instantiate it using the constructor and start its thread.

```cpp
#include "broker.h"

insideJob::Broker broker();
broker.start();
```

Then pass the broker to all the threads by pointer or reference.

In order to stop the broker call `broker.stop()` or let it destruct at the end of the scope.

## Subscribe

To subscribe to the `Broker`, create a `Subscriber` object by calling its constructor and passing the main `Broker` as an argument.

```cpp
#include "subscriber.h"

insideJob::Subscriber sub{broker};
```

To subscribe to a topic, use the `subscribe` method. You can subscribe to as many topics as you need.

```cpp
std::string topic{"/base/some1"};
sub.subscribe(topic);
```

You can check if messages have been received or determine the number of received messages using the `queueSize()` and `queueEmpty()` methods.

To retrieve the data, use the `popData()` method.

## Publish

To publish messages to the `Broker`, create a `Publisher` object by calling its constructor and passing the main `Broker` as an argument.

```cpp
#include "publisher.h"

insideJob::Publisher pub{broker};
```

To publish a message, use the `publish` method, providing the topic and the data.

```cpp
Data packet{.num = 12, .str = "test"};
pub.publish(topic, (void*)&packet, 64);
```
