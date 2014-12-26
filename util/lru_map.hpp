#ifndef POLARITY_UTIL_LRU_MAP_HPP__
#define POLARITY_UTIL_LRU_MAP_HPP__

#include <unordered_map>
#include <list>

namespace Polarity {

template <class Key, class Value, class Hasher=std::hash<Key>>
class LRUMap {
    typedef std::pair<Key, Value> KeyValue;
    typedef std::list<KeyValue> ValueList;
    typedef std::unordered_map<Key, typename ValueList::iterator, Hasher> IterMap;
public:
    LRUMap(size_t capacity_)
        : capacity(capacity_) {
    }

    void setCapacity(size_t new_capacity) {
        capacity = new_capacity;
        while (value_list.size() > new_capacity) {
            pop();
        }
    }

    void clear() {
        while (!value_list.empty()) {
            pop();
        }
    }

    void insert(const Key &key, Value &&value) {
        value_list.emplace_front(KeyValue(key,std::move(value)));
        insertFrontToMap(key);
    }

    void insert(const Key &key, const Value &value) {
        value_list.push_front(KeyValue(key, value));
        insertFrontToMap(key);
    }

    bool remove(const Key &key) {
        auto it = iter_map.find(key);
        if (it != iter_map.end()) {
            value_list.erase(it->second);
            iter_map.erase(it);
            return true;
        }
        return false;
    }

    Value *get(const Key &key) {
        typename IterMap::iterator iter = iter_map.find(key);
        if (iter == iter_map.end()) {
            return nullptr;
        }
        value_list.splice(value_list.begin(), value_list, iter->second);
        return &(iter->second->second);
    }

private:

    void insertFrontToMap(const Key &key) {
        auto ret = iter_map.insert(make_pair(key, value_list.begin()));
        if (!ret.second) {
            value_list.erase(ret.first->second);
            ret.first->second = value_list.begin();
        }

        if (value_list.size() > capacity) {
            pop();
        }
    }

    void pop() {
        remove(value_list.back().first);
    }

    size_t capacity;
    ValueList value_list;
    IterMap iter_map;
};

}
#endif

