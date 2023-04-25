#pragma once

#include <string>
#include <tuple>

#include "hashmap_tree.h"

#ifndef BROKER_QUEUE_SIZE
#define BROKER_QUEUE_SIZE 4096
#endif

namespace insideJob
{

typedef size_t handle;
typedef bool (*Callback)(void* data, size_t len);

class Broker
{
   public:
    Broker();
    ~Broker();

    handle connect();

    bool publish(const std::string& topic, void* data, size_t len);

    void subscribe(const std::string& topic, const handle& hand, Callback callback);
    void remove_sub(const std::string& topic, const handle& hand);

    void start();

    void printBuffer();

   private:
    static void entry(Broker* that);

    void run();

    std::vector<std::string> parseTopic(const std::string& topic);

    int getQueueHead();

    bool isQueueFull();

    size_t queueSize();

    bool _should_run = true;

    // instead of just callback - hash of handle and callback for identification
    HashmapTree<std::string, std::pair<handle, Callback>> _tree;

    size_t _queue_tail = 0; /* where broker use */
    size_t _queue_head = 0; /* where new objects pushed */
    std::tuple<std::string, void*, size_t> _buffer[BROKER_QUEUE_SIZE];
};

}  // namespace insideJob
