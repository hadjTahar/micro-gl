#pragma once

template<typename T, unsigned N>
class static_array {
public:
    using index = unsigned int;
    using type = T;
    static constexpr capacity = N;

    explicit static_array() = default;

    template<class Iterable>
    static_array(const Iterable & list) {
        for (const auto & item : list)
            push_back(item);
    }

    static_array(const static_array & container) {
        for(auto ix = 0; ix < container.size(); ix++)
            this->push_back(container[ix]);
    }

    static_array & operator=(const static_array & container) {
        this->clear();
        for(index ix = 0; ix < container.size(); ix++)
            this->push_back(container[ix]);
        return (*this);
    }

    T& operator[](index i) { return _data[i]; }
    const T& operator[](index i) const { return _data[i]; }
    T* data() { return _data; }
    const T* data() const { return _data; }
    const T& peek() { return operator[](_current); }

    void push_back(const T & v) {
        if(_current==N-1) return;
        _data[_current++] = v;
    }

    void pop_back() {
        if(_current==0) return;
        _data[_current--].~T();
    }

    void clear() { _current = 0; }
    index size() const { return _current; }
    constexpr index capacity() const { return N; }

    T* begin() { return _data; }
    T* end() { return _data + _current; }

private:
    T _data[N];
    index _current = 0;
};
