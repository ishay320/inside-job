#include <assert.h>

#include <chrono>
#include <cstddef>
#include <queue>
#include <string>
#include <thread>
#include <tuple>

#include "hashmap_tree.h"

using namespace std::chrono_literals;

typedef size_t handle;
typedef bool (*Callback)(void* data, size_t len);

template <size_t buffer_size>
class Broker
{
   public:
    Broker() {}
    ~Broker() {}

    handle connect()
    {
        static handle id = 0;
        return id++;
    }

    bool publish(const std::string& topic, void* data, size_t len)
    {
        int queue_pos = getQueueHead();
        if (queue_pos == -1)
        {
            std::cout << "WARNING: broker buffer is full\n";
            return false;
        }

        _buffer[queue_pos] = std::make_tuple(topic, data, len);
        return true;
    }

    void subscribe(const std::string& topic, const handle& hand, Callback callback)
    {
        if (topic.empty())
        {
            std::cout << "ERROR: topic is empty, cannot subscribe\n";
            return;
        }

        std::vector<std::string> parsed_topic;
        parsed_topic = parseTopic(topic);

        _tree.insert(parsed_topic.data(), parsed_topic.size(), std::pair<handle, Callback>{hand, callback});
    }
    void remove_sub(const std::string& topic, const handle& hand);

    void start()
    {
        std::thread thread(entry, this);
        thread.detach();
    }

    void printBuffer()
    {
        for (size_t i = _queue_tail; i != _queue_head; i = (i + 1) % buffer_size)
        {
            const auto [f, s, l] = _buffer[i];
            std::cout << "pos: " << i << " data: " << f << " num: " << l << '\n';
        }
    }

   private:
    static void entry(Broker<buffer_size>* that) { that->run(); }

    void run()
    {
        while (_should_run)
        {
            // sleep if buffer empty
            if (queueSize() == 0)
            {
                // TODO: replace with semaphore
                std::this_thread::sleep_for(10ms);
                continue;
            }

            // pull from buffer
            const auto [str, data, len] = _buffer[_queue_tail];

            // release queue space
            _queue_tail = (_queue_tail + 1) % buffer_size;

            // parse
            auto topic = parseTopic(str);

            // get clients from tree
            std::vector<std::pair<handle, Callback>> clients = _tree.get(topic.data(), topic.size());

            // push
            for (auto&& client : clients)
            {
                client.second(data, len);
            }
        }
    }

    std::vector<std::string> parseTopic(const std::string& topic)
    {
        constexpr char delimiter = '/';
        std::vector<std::string> parsed_topic;

        size_t last = 0;
        size_t next = 0;
        while ((next = topic.find(delimiter, last)) != std::string::npos)
        {
            parsed_topic.push_back(topic.substr(last, next - last));
            last = next + 1;
        }
        parsed_topic.push_back(topic.substr(last));
        return parsed_topic;
    }

    int getQueueHead()
    {
        if (isQueueFull())
        {
            return -1;
        }
        size_t ret  = _queue_head;
        _queue_head = (_queue_head + 1) % buffer_size;
        return ret;
    }

    bool isQueueFull()
    {
        // with this approach im losing one space, another option is to use bool flag
        return queueSize() == (buffer_size - 1);
    }

    size_t queueSize()
    {
        size_t queue_size = 0;
        if (_queue_head < _queue_tail)
        {
            queue_size = _queue_head + buffer_size - _queue_tail;
        }
        else
        {
            queue_size = _queue_head - _queue_tail;
        }
        return queue_size;
    }

    bool _should_run = true;

    HashmapTree<std::string, std::pair<handle, Callback>> _tree;  // instead of callback - hash of handle and callback

    size_t _queue_tail = 0; /* where broker use */
    size_t _queue_head = 0; /* where new objects pushed */
    std::tuple<std::string, void*, size_t> _buffer[buffer_size];
};
