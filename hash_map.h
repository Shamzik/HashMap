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
    typedef typename list::iterator list_iterator;
    typedef typename list::const_iterator list_const_iterator;
    typedef std::size_t size_t;

    Hash _hash_function;
    list _data;
    std::vector<list_iterator> _iterators;
    std::vector<size_t> _sizes;
    size_t _size;
    static const size_t KOEFF = 19, DELTA = 17;

    void resize(const size_t& size) {
        list oldData = _data;
        _data.clear();
        _iterators.clear();
        _iterators.resize(size * KOEFF, _data.end());
        _sizes.clear();
        _sizes.resize(_iterators.size());
        for (pointer ptr : oldData) {
            size_t hash = _hash_function(ptr->first) % _iterators.size();
            _iterators[hash] = _data.insert(_iterators[hash], ptr);
            ++_sizes[hash];
        }
    }

    void insert_without_resize(const value_type& p) {
        size_t hash = _hash_function(p.first) % _iterators.size();
        size_t index = 0;
        list_iterator it = _iterators[hash];
        while (index < _sizes[hash]) {
            if ((*it)->first == p.first) {
                break;
            }
            ++it;
            ++index;
        }
        if (index == _sizes[hash]) {
            _iterators[hash] = _data.insert(_iterators[hash], new value_type(p));
            ++_sizes[hash];
            ++_size;
        }
    }

    void insert_without_checking(const value_type& p) {
        size_t hash = _hash_function(p.first) % _iterators.size();
        _iterators[hash] = _data.insert(_iterators[hash], new value_type(p));
        ++_sizes[hash];
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
        if (size() * KOEFF * DELTA < _iterators.size()) {
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
        if (size() * KOEFF * DELTA < _iterators.size()) {
            resize(size());
        }
    }

    HashMap(const HashMap& other)
        : _hash_function(other.hash_function())
        , _size(0)
    {
        if (other.size() * KOEFF > _iterators.size() * DELTA) {
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
        if (other.size() * KOEFF > _iterators.size() * DELTA) {
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
        return _data.empty();
    }

    Hash hash_function() const {
        return _hash_function;
    }

    void insert(const value_type& p) {
        if ((size() + 1) * KOEFF > DELTA * _iterators.size()) {
            resize(size() + 1);
        }
        insert_without_resize(p);
    }

    void erase(const KeyType& key) {
        if (empty()) {
            return;
        }
        size_t hash = _hash_function(key) % _iterators.size();
        size_t index = 0;
        list_iterator it = _iterators[hash];
        while (index < _sizes[hash]) {
            if ((*it)->first == key) {
                break;
            }
            ++it;
            ++index;
        }
        if (index != _sizes[hash]) {
            delete *it;
            if (index == 0) {
                if (_sizes[hash] > 1) {
                    ++_iterators[hash];
                } else {
                    _iterators[hash] = _data.end();
                }
            }
            _data.erase(it);
            --_sizes[hash];
            --_size;
            if (size() * KOEFF * DELTA < _iterators.size()) {
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

        list_const_iterator it;

        const_iterator()
        {}

        const_iterator(list_const_iterator it)
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
        return _data.begin();
    }

    iterator end() {
        return _data.end();
    }

    const_iterator begin() const {
        return _data.begin();
    }

    const_iterator end() const {
        return _data.end();
    }

    iterator find(const KeyType& key) {
        if (empty()) {
            return end();
        }
        size_t hash = _hash_function(key) % _iterators.size();
        size_t index = 0;
        list_iterator it = _iterators[hash];
        while (index < _sizes[hash]) {
            if ((*it)->first == key) {
                break;
            }
            ++it;
            ++index;
        }
        return (index == _sizes[hash] ? end() : it);
    }

    const_iterator find(const KeyType& key) const {
        if (empty()) {
            return end();
        }
        size_t hash = _hash_function(key) % _iterators.size();
        size_t index = 0;
        list_iterator it = _iterators[hash];
        while (index < _sizes[hash]) {
            if ((*it)->first == key) {
                break;
            }
            ++it;
            ++index;
        }
        if (index == _sizes[hash]) {return end();}
        return list_const_iterator(it);
    }

    ValueType& operator[](const KeyType& key) {
        iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        if ((size() + 1) * KOEFF > DELTA * _iterators.size()) {
            resize(size() + 1);
        }
        size_t hash = _hash_function(key) % _iterators.size();
        _iterators[hash] =
            _data.insert(_iterators[hash], new value_type(key, ValueType()));
        ++_sizes[hash];
        ++_size;
        return (*_iterators[hash])->second;
    }

    const ValueType& at(const KeyType& key) const {
        const_iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        throw std::out_of_range("out of range in HashMap::at");
    }

    void clear() {
        for (pointer ptr : _data) {
            delete ptr;
        }
        _data.clear();
        _iterators.clear();
        _sizes.clear();
        _size = 0;
    }
};
