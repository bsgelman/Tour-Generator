#ifndef HASHMAP_H
#define HASHMAP_H

#include <vector>
#include <list>
#include <functional>
#include <string>
#include <iostream>


template <typename T>
class HashMap
{
public:
    HashMap(double max_load = 0.75)
        : nItems(0), maxLoadFactor(max_load) {
        buckets.resize(10);
    }

    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;

    ~HashMap() {} // destructor; deletes all of the items in the hashmap
    int size() const { // return the number of associations in the hashmap
        return nItems;
    }
    // The insert method associates one item (key) with another (value).
    // If no association currently exists with that key, this method inserts
    // a new association into the hashmap with that key/value pair. If there is
    // already an association with that key in the hashmap, then the item
    // associated with that key is replaced by the second parameter (value).
    // Thus, the hashmap must contain no duplicate keys.
    void insert(const std::string& key, const T& value) {
        size_t index = std::hash<std::string>()(key) % buckets.size();
        for (typename std::list<Node>::iterator p = buckets[index].begin(); p != buckets[index].end(); p++) {
            Node& node = *p;
            if (node.m_key == key) {
                node.m_value = value;
                return;
            }
        }

        // add new node
        buckets[index].push_front(Node(key, value));
        nItems++;

        // check if rehash is necessary
        double num = nItems;
        double loadFactor = num / buckets.size();
        if (loadFactor > maxLoadFactor) {
            rehash();
        }
    }
    // Defines the bracket operator for HashMap, so you can use your map like this:
    // your_map["david"] = 2.99;
    // If the key does not exist in the hashmap, this will create a new entry in
    // the hashmap and map it to the default value of type T (0 for builtin types).
    // It returns a reference to the newly created value in the map.
    T& operator[](const std::string& key) {
        size_t index = std::hash<std::string>()(key) % buckets.size();
        for (typename std::list<Node>::iterator p = buckets[index].begin(); p != buckets[index].end(); p++) {
            Node& node = *p;    
            if (node.m_key == key) {
                return node.m_value;
            }
        }

        insert(key, T());
        index = std::hash<std::string>()(key) % buckets.size();
        for (typename std::list<Node>::iterator p = buckets[index].begin(); p != buckets[index].end(); p++) {
            Node& node = *p;
            if (node.m_key == key) {
                return node.m_value;
            }
        }

        throw std::runtime_error("Key not found after insert"); // can remove later, use for testing
    }
    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value within the map.
    const T* find(const std::string& key) const {
        size_t index = std::hash<std::string>()(key) % buckets.size();
        for (typename std::list<Node>::const_iterator p = buckets[index].begin(); p != buckets[index].end(); p++) {
            const Node& node = *p;
            if (node.m_key == key) {
                return &node.m_value;
            }
        }
        return nullptr;
    }
    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value or modify it directly within the map.
    T* find(const std::string& key) {
        const auto& hm = *this;
        return const_cast<T*>(hm.find(key));
    }
private:
    struct Node {
        std::string m_key;
        T m_value;
        Node(const std::string& k, const T& v) : m_key(k), m_value(v) {}
    };

    std::vector<std::list<Node>> buckets;
    size_t nItems;
    double maxLoadFactor;

    void rehash() {
        size_t increasedNumBuckets = buckets.size() * 2;
        std::vector<std::list<Node>> newBuckets(increasedNumBuckets);

        for (typename std::vector<std::list<Node>>::iterator p = buckets.begin(); p != buckets.end(); p++) { // for each bucket
            std::list<Node>& bucket = *p;
            for (typename std::list<Node>::iterator q = bucket.begin(); q != bucket.end(); q++) { // for each node in that bucket
                Node& node = *q;
                size_t index = std::hash<std::string>()(node.m_key) % increasedNumBuckets; // rehash index
                newBuckets[index].push_back(node); // add that node with its new index to the other vector
            }
        }

        buckets = std::move(newBuckets); // cannibalizes "newBuckets" and pretty much makes buckets point to p
    }
};

#endif