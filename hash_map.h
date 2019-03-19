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
	typedef std::pair<const KeyType, ValueType> value_type;
	typedef value_type* pointer;
	typedef std::list<pointer> list;
	typedef typename list::iterator listIterator;
	typedef typename list::const_iterator listConstIterator;
	typedef std::size_t size_t;

    Hash _hash_function;
    list data;
    std::vector<listIterator> iterators;
    std::vector<size_t> sizes;
    size_t _size;
    static const size_t KOEFF = 19, DELTA = 17;

    void resize(const size_t& size) {
        list oldData = data;
        data.clear();
        iterators.clear();
        iterators.resize(size * KOEFF, data.end());
        sizes.clear();
        sizes.resize(iterators.size());
        for (pointer ptr : oldData) {
            size_t hash = _hash_function(ptr->first) % iterators.size();
            iterators[hash] = data.insert(iterators[hash], ptr);
            ++sizes[hash];
        }
    }

    void insert_without_resize(const value_type& p) {
        size_t hash = _hash_function(p.first) % iterators.size();
        size_t i = 0;
        listIterator it = iterators[hash];
        while (i < sizes[hash]) {
            if ((*it)->first == p.first) {
                break;
            }
            ++it;
            ++i;
        }
        if (i == sizes[hash]) {
            iterators[hash] = data.insert(iterators[hash], new value_type(p));
            ++sizes[hash];
            ++_size;
        }
    }

    void insert_without_checking(const value_type& p) {
        size_t hash = _hash_function(p.first) % iterators.size();
        iterators[hash] = data.insert(iterators[hash], new value_type(p));
        ++sizes[hash];
        ++_size;
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
        , _size(0)
    {}

    template
    <typename Iterator>
    HashMap(Iterator begin, Iterator end, Hash hash_function = Hash())
        : _hash_function(hash_function)
        , _size(0)
    {
        size_t possibleSize = 0;
        for (Iterator i = begin; i != end; ++i) {
            ++possibleSize;
        }
        if (possibleSize != 0) {
            resize(possibleSize);
        }
        insert_without_resize(begin, end);
        if (size() * KOEFF * DELTA < iterators.size()) {
            resize(size());
        }
    }

    HashMap(const std::initializer_list<value_type >& list,
            const Hash& hash_function = Hash())
        : _hash_function(hash_function)
        , _size(0)
    {
        if (list.size() != 0) {
            resize(list.size());
        }
        insert_without_resize(list.begin(), list.end());
        if (size() * KOEFF * DELTA < iterators.size()) {
            resize(size());
        }
    }

    HashMap(const HashMap& other)
        : _hash_function(other.hash_function())
        , _size(0)
    {
        if (other.size() * KOEFF > iterators.size() * DELTA) {
            resize(other.size());
        }
        for (const value_type& p : other) {
            insert_without_checking(p);
        }
    }

    HashMap& operator=(const HashMap& other) {
        if (this == &other) {
            return *this;
        }
        clear();
        _hash_function = other.hash_function();
        if (other.size() * KOEFF > iterators.size() * DELTA) {
            resize(other.size());
        }
        for (const value_type& p : other) {
            insert_without_checking(p);
        }
        return *this;
    }

    ~HashMap() {
        clear();
    }

    size_t size() const {
        return _size;
    }

    bool empty() const {
        return data.empty();
    }

    Hash hash_function() const {
        return _hash_function;
    }

    void insert(const value_type& p) {
        if ((size() + 1) * KOEFF > DELTA * iterators.size()) {
            resize(size() + 1);
        }
        insert_without_resize(p);
    }

    void erase(const KeyType& key) {
        if (empty()) {
            return;
        }
        size_t hash = _hash_function(key) % iterators.size();
        size_t i = 0;
        listIterator it = iterators[hash];
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
                    ++iterators[hash];
                } else {
                    iterators[hash] = data.end();
                }
            }
            data.erase(it);
            --sizes[hash];
            --_size;
            if (size() * KOEFF * DELTA < iterators.size()) {
                resize(size());
            }
        }
    }

    struct iterator {
        typedef void difference_type;
        typedef value_type& reference;
        typedef std::forward_iterator_tag iterator_category;

        typename list::iterator it;

        iterator()
        {}

        iterator(typename list::iterator it)
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
        typedef void difference_type;
        typedef value_type& reference;
        typedef std::forward_iterator_tag iterator_category;

        listConstIterator it;

        const_iterator()
        {}

        const_iterator(listConstIterator it)
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
        return data.begin();
    }

    iterator end() {
        return data.end();
    }

    const_iterator begin() const {
        return data.begin();
    }

    const_iterator end() const {
        return data.end();
    }

    iterator find(const KeyType& key) {
        if (empty()) {
            return end();
        }
        size_t hash = _hash_function(key) % iterators.size();
        size_t i = 0;
        listIterator it = iterators[hash];
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
        size_t hash = _hash_function(key) % iterators.size();
        size_t i = 0;
        listIterator it = iterators[hash];
        while (i < sizes[hash]) {
            if ((*it)->first == key) {
                break;
            }
            ++it;
            ++i;
        }
        if (i == sizes[hash]) {return end();}
        return listConstIterator(it);
    }

    ValueType& operator[](const KeyType& key) {
        iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        if ((size() + 1) * KOEFF > DELTA * iterators.size()) {
            resize(size() + 1);
        }
        size_t hash = _hash_function(key) % iterators.size();
        iterators[hash] =
            data.insert(iterators[hash], new value_type(key, ValueType()));
        ++sizes[hash];
        ++_size;
        return (*iterators[hash])->second;
    }

    const ValueType& at(const KeyType& key) const {
        const_iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        throw std::out_of_range("out of range in HashMap::at");
    }

    void clear() {
        for (pointer ptr : data) {
            delete ptr;
        }
        data.clear();
        iterators.clear();
        sizes.clear();
        _size = 0;
    }
};
