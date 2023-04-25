#pragma once

#include <string>
#include <unordered_map>
#include <vector>

template <class K, class V>
class HashmapTree
{
   public:
    HashmapTree() {}
    ~HashmapTree()
    {
        for (auto& [key, value] : _tree)
        {
            delete value;
        }
    }

    typedef bool (*cmpFunc)(V a, V b);
    bool remove(const K* topic, V value, cmpFunc cmp);  // TODO: Implement this

    bool insert(const K* topic, size_t len, V data)
    {
        if (len == 0)
        {
            _data.push_back(data);
            return true;
        }

        if (auto search = _tree.find(topic[0]); search != _tree.end())
        {
            return search->second->insert(topic + 1, len - 1, data);
        }

        HashmapTree* node = new HashmapTree();
        _tree.emplace(topic[0], node);
        return node->insert(topic + 1, len - 1, data);
    }

    std::vector<V> get(const K* topic, size_t len)
    {
        std::vector<V> out;
        get_rec(topic, len, out);
        return out;
    }

   private:
    bool get_rec(const K* topic, size_t len, std::vector<V>& out)
    {
        if (len == 0)
        {
            out.insert(out.end(), _data.begin(), _data.end());
            for (const auto& [key, value] : _tree)
            {
                if (!value->get_rec(NULL, 0, out))
                {
                    return false;
                }
            }
            return true;
        }

        if (auto search = _tree.find(topic[0]); search != _tree.end())
        {
            return search->second->get_rec(topic + 1, len - 1, out);
        }
        return false;
    }

    std::unordered_map<K, HashmapTree*> _tree;
    std::vector<V> _data;
};
