#pragma once

#include <mutex>
#include <queue>

namespace insideJob
{

template <class T, size_t max_size>
class CircularQueue
{
public:
    CircularQueue() {}
    ~CircularQueue()
    {
        for (size_t i = 0; i < _data.size(); i++)
        {
            _data.pop();
        }
    }

    bool push(const T& item)
    {
        _data_mutex.lock();
        if (_data.size() == max_size)
        {
            _data_mutex.unlock();
            return false;
        }
        _data.push(item);
        _data_mutex.unlock();
        return true;
    }

    T peak()
    {
        std::lock_guard lock{_data_mutex};
        return _data.front();
    }

    T pop()
    {
        std::lock_guard lock{_data_mutex};
        auto tmp = _data.front();
        _data.pop();
        return tmp;
    }

    bool empty() const
    {
        std::lock_guard lock{_data_mutex};
        return _data.empty();
    }

    size_t size() const
    {
        std::lock_guard lock{_data_mutex};
        return _data.size();
    }

private:
    mutable std::mutex _data_mutex;
    std::queue<T> _data;
};

} // namespace insideJob