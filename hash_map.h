#include <algorithm>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <list>
#include <stdexcept>
#include <utility>
#include <vector>

template
<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
public:
    Hash _hash_function;
    std::list<std::pair<const KeyType, ValueType>*> data1;
    std::vector<typename std::list<std::pair<const KeyType, ValueType>*>::iterator> data;
    std::vector<size_t> sizes;
    std::size_t __size;
    static const size_t k = 19, d = 17;

    void resize(const std::size_t& size) {
        std::list<std::pair<const KeyType, ValueType>*> _data1 = data1;
        data1.clear();
        data.clear();
        data.resize(size * k, data1.end());
        sizes.clear();
        sizes.resize(data.size());
        for (std::pair<const KeyType, ValueType>* ptr : _data1) {
            std::size_t hash = _hash_function(ptr->first) % data.size();
            data[hash] = data1.insert(data[hash], ptr);
            ++sizes[hash];
        }
    }

    void insert_without_resize(const std::pair<const KeyType, ValueType>& p) {
        std::size_t hash = _hash_function(p.first) % data.size();
        size_t i = 0;
        typename std::list<std::pair<const KeyType, ValueType>*>::iterator it = data[hash];
        while (i < sizes[hash]) {
            if ((*it)->first == p.first) {
                break;
            }
            ++it;
            ++i;
        }
        if (i == sizes[hash]) {
            data[hash] = data1.insert(data[hash], new std::pair<const KeyType, ValueType>(p));
            ++sizes[hash];
            ++__size;
        }
    }

    void insert_without_checking(const std::pair<const KeyType, ValueType>& p) {
        std::size_t hash = _hash_function(p.first) % data.size();
        data[hash] = data1.insert(data[hash], new std::pair<const KeyType, ValueType>(p));
        ++sizes[hash];
        ++__size;
    }

    template
    <typename Iterator>
    void insert_without_resize(Iterator begin, Iterator end) {
        for (Iterator i = begin; i != end; ++i) {
            insert_without_resize(*i);
        }
    }

    HashMap(Hash hash_function = Hash())
        : _hash_function(hash_function)
        , __size(0)
    {}

    template
    <typename Iterator>
    HashMap(Iterator begin, Iterator end, Hash hash_function = Hash())
        : _hash_function(hash_function)
        , __size(0)
    {
        std::size_t _size = 0;
        for (Iterator i = begin; i != end; ++i) {
            ++_size;
        }
        if (_size != 0) {
            resize(_size);
        }
        insert_without_resize(begin, end);
        if (size() * k * d < data.size()) {
            resize(size());
        }
    }

    HashMap(const std::initializer_list<std::pair<const KeyType, ValueType>>& list,
            const Hash& hash_function = Hash())
        : _hash_function(hash_function)
        , __size(0)
    {
        if (list.size() != 0) {
            resize(list.size());
        }
        insert_without_resize(list.begin(), list.end());
        if (size() * k * d < data.size()) {
            resize(size());
        }
    }

    HashMap(const HashMap& other)
        : _hash_function(other.hash_function())
        , __size(0)
    {
        if (other.size() * k > data.size() * d) {
            resize(other.size());
        }
        for (const std::pair<const KeyType, ValueType>& p : other) {
            insert_without_checking(p);
        }
    }

    HashMap& operator=(const HashMap& other) {
        if (this == &other) {
            return *this;
        }
        clear();
        _hash_function = other.hash_function();
        if (other.size() * k > data.size() * d) {
            resize(other.size());
        }
        for (const std::pair<const KeyType, ValueType>& p : other) {
            insert_without_checking(p);
        }
        return *this;
    }

    ~HashMap() {
        clear();
    }

    std::size_t size() const {
        return __size;
    }

    bool empty() const {
        return data1.empty();
    }

    Hash hash_function() const {
        return _hash_function;
    }

    void insert(const std::pair<const KeyType, ValueType>& p) {
        if ((size() + 1) * k > d * data.size()) {
            resize(size() + 1);
        }
        insert_without_resize(p);
    }

    void erase(const KeyType& key) {
        if (empty()) {
            return;
        }
        std::size_t hash = _hash_function(key) % data.size();
        size_t i = 0;
        typename std::list<std::pair<const KeyType, ValueType>*>::iterator it = data[hash];
        while (i < sizes[hash]) {
            if ((*it)->first == key) {
                break;
            }
            ++it;
            ++i;
        }
        if (i != sizes[hash]) {
            delete *it;
            if (i == 0) {
                if (sizes[hash] > 1) {
                    ++data[hash];
                } else {
                    data[hash] = data1.end();
                }
            }
            data1.erase(it);
            --sizes[hash];
            --__size;
            if (size() * k * d < data.size()) {
                resize(size());
            }
        }
    }

    struct iterator {
        typedef std::pair<const KeyType, ValueType> value_type;
        typedef void difference_type;
        typedef value_type* pointer;
        typedef value_type& reference;
        typedef std::forward_iterator_tag iterator_category;

        typename std::list<value_type*>::iterator it;

        iterator()
        {}

        iterator(typename std::list<value_type*>::iterator it)
            : it(it)
        {}

        bool operator==(iterator other) {
            return it == other.it;
        }

        bool operator!=(iterator other) {
            return !(*this == other);
        }

        reference operator*() {
            return **it;
        }

        pointer operator->() {
            return *it;
        }

        iterator& operator++() {
            ++it;
            return *this;
        }

        iterator operator++(int) {
            iterator clone = *this;
            ++(*this);
            return clone;
        }
    };

    struct const_iterator {
        typedef std::pair<const KeyType, ValueType> value_type;
        typedef void difference_type;
        typedef value_type* pointer;
        typedef value_type& reference;
        typedef std::forward_iterator_tag iterator_category;

        typename std::list<value_type*>::const_iterator it;

        const_iterator()
        {}

        const_iterator(typename std::list<value_type*>::const_iterator it)
            : it(it)
        {}

        bool operator==(const_iterator other) {
            return it == other.it;
        }

        bool operator!=(const_iterator other) {
            return !(*this == other);
        }

        const value_type& operator*() {
            return **it;
        }

        const value_type* operator->() {
            return *it;
        }

        const_iterator& operator++() {
            ++it;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator clone = *this;
            ++(*this);
            return clone;
        }
    };

    iterator begin() {
        return data1.begin();
    }

    iterator end() {
        return data1.end();
    }

    const_iterator begin() const {
        return data1.begin();
    }

    const_iterator end() const {
        return data1.end();
    }

    iterator find(const KeyType& key) {
        if (empty()) {
            return end();
        }
        std::size_t hash = _hash_function(key) % data.size();
        size_t i = 0;
        typename std::list<std::pair<const KeyType, ValueType>*>::iterator it = data[hash];
        while (i < sizes[hash]) {
            if ((*it)->first == key) {
                break;
            }
            ++it;
            ++i;
        }
        return (i == sizes[hash] ? end() : it);
    }

    const_iterator find(const KeyType& key) const {
        if (empty()) {
            return end();
        }
        std::size_t hash = _hash_function(key) % data.size();
        size_t i = 0;
        typename std::list<std::pair<const KeyType, ValueType>*>::iterator it = data[hash];
        while (i < sizes[hash]) {
            if ((*it)->first == key) {
                break;
            }
            ++it;
            ++i;
        }
        if (i == sizes[hash]) {return end();}
        return typename std::list<std::pair<const KeyType, ValueType>*>::const_iterator(it);
    }

    ValueType& operator[](const KeyType& key) {
        iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        if ((size() + 1) * k > d * data.size()) {
            resize(size() + 1);
        }
        std::size_t hash = _hash_function(key) % data.size();
        data[hash] =
            data1.insert(data[hash], new std::pair<const KeyType, ValueType>(key, ValueType()));
        ++sizes[hash];
        ++__size;
        return (*data[hash])->second;
    }

    const ValueType& at(const KeyType& key) const {
        const_iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        throw std::out_of_range("out of range in HashMap::at");
    }

    void clear() {
        for (std::pair<const KeyType, ValueType>* ptr : data1) {
            delete ptr;
        }
        data1.clear();
        data.clear();
        sizes.clear();
        __size = 0;
    }
};
