#pragma once

#include <array>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <semaphore.h>
#include <string>
#include <thread>
#include <vector>

#include "circular_queue.h"
#include "hashmap_tree.h"

#ifndef BROKER_QUEUE_SIZE
#define BROKER_QUEUE_SIZE 4096
#endif

namespace insideJob
{

typedef size_t handle;
typedef std::function<bool(std::shared_ptr<void>, size_t)> Callback;

class Broker
{
public:
    Broker();
    ~Broker();

    handle connect();

    bool publish(const std::string& topic, void* data, size_t len);

    void subscribe(const std::string& topic, const handle& hand, Callback callback);
    bool removeSub(const std::string& topic, const handle& hand);

    void start();
    void stop();

    void printBuffer();
    void printTree();

private:
    static void entry(Broker* that);

    void run();

    std::vector<std::string> parseTopic(const std::string& topic);

    bool isQueueFull();

    size_t queueSize();

    std::thread _thread;
    std::atomic<bool> _should_run = true;

    // hash of handle and callback for identification
    std::mutex _tree_mutex;
    HashmapTree<std::string, std::pair<handle, Callback>> _tree;

    typedef struct
    {
        std::vector<std::string> parsed_topic;
        std::shared_ptr<void> data_share;
        size_t len;
    } BufferItem;

    sem_t _work;
    CircularQueue<BufferItem, BROKER_QUEUE_SIZE> _buffer;
};

} // namespace insideJob
