#pragma once

#include <unordered_map>
#include <vector>

namespace insideJob
{
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

    bool remove(const K* topic, size_t len, V value, cmpFunc cmp)
    {
        if (len == 0)
        {
            return false;
        }

        // get the node
        auto node = _tree.find(topic[0]);
        if (node == _tree.end())
        {
            return false;
        }
        for (size_t i = 1; i < len; i++)
        {
            node = node->second->_tree.find(topic[i]);
            if (node == _tree.end())
            {
                return false;
            }
        }

        // remove the value
        auto data_point = node->second->_data.end();
        for (auto&& data = node->second->_data.begin(); data != node->second->_data.end(); data++)
        {
            if (cmp(*data, value))
            {
                data_point = data;
                break;
            }
        }
        if (data_point != node->second->_data.end())
        {
            node->second->_data.erase(data_point);
        }
        else
        {
            return false;
        }

        // if node empty remove topic
        if (node->second->_data.empty())
        {
            // TODO: fix it from the parent recursively
            // delete node;
        }

        return true;
    }

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

    std::vector<V> getDown(const K* topic, size_t len)
    {
        std::vector<V> out;
        get_rec(topic, len, out);
        return out;
    }

    std::vector<V> getPath(const K* topic, size_t len) // TODO: replace with vector
    {
        std::vector<V> out;

        // Insert the root data
        out.insert(out.end(), _data.begin(), _data.end());

        HashmapTree* tree_pos = this;
        for (size_t i = 0; i < len; i++)
        {
            auto search = tree_pos->_tree.find(topic[i]);
            if (search == tree_pos->_tree.end())
            {
                break;
            }
            tree_pos = search->second;

            out.insert(out.end(), tree_pos->_data.begin(), tree_pos->_data.end());
        }

        return out;
    }

    void printTopics(void (*print_fun)(K))
    {
        for (auto&& n : _tree)
        {
            print_fun(n.first);
            n.second->printTopics(print_fun);
        }
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
} // namespace insideJob
