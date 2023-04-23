#include <cstddef>
#include <queue>
#include <string>
#include <tuple>

#include "hashmap_tree.h"

typedef size_t handle;
typedef bool (*Callback)(void* data, size_t len);

template <size_t buffer_size>
class Broker
{
   public:
    Broker();
    ~Broker();

    handle connect()
    {
        static handle id = 0;
        return id++;
    }

    bool publish(const std::string& topic, void* data, size_t len)
    {
        int queue_pos = getQueueNumber();
        if (queue_pos = -1)
        {
            return false;
        }

        _buffer[queue_pos] = std::tuple<std::string, void*, size_t>{topic, data, len};
    }

    void subscribe(const std::string& topic, const handle& hand, Callback callback)
    {
        std::vector<std::string> parsed_topic;

        _tree.insert(parsed_topic.data(), parsed_topic.size(), std::pair<handle, Callback>{hand, callback});
    }
    void remove_sub(const std::string& topic, const handle& hand);

    static void run()
    {
        assert(false && "function not implemented yet");
        while (_should_run)
        {
            // sleep if buffer empty

            // pull from buffer
            // parse
            // get clients from tree
            // push
        }
    }

   private:
    int getQueueNumber()
    {
        if (isQueueFull())
        {
            return -1;
        }
        size_t ret  = _queue_head;
        _queue_head = (_queue_head + 1) % buffer_size;
        return ret;
    }

    bool isQueueFull() { return queueSize() == buffer_size; }

    size_t queueSize()
    {
        size_t queue_size = 0;
        if (_queue_head < _queue_tail)
        {
            queue_size = _queue_head + _queue_tail - buffer_size;
        }
        else
        {
            queue_size = _queue_tail - _queue_head;
        }
        return queue_size;
    }

    bool _should_run = true;

    HashmapTree<std::string, std::pair<handle, Callback>> _tree;  // instead of callback - hash of handle and callback

    size_t _queue_tail = 0; /* where broker use */
    size_t _queue_head = 0; /* where new objects pushed */
    std::tuple<std::string, void*, size_t> _buffer[buffer_size];
};
