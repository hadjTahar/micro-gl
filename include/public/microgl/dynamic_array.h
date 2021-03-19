#pragma once

template<typename T>
class dynamic_array {
    using dynamic_array_ref = dynamic_array<T> &;
    using const_dynamic_array_ref = const dynamic_array<T> &;
public:
    using index = unsigned int;
    using type = T;

    template<class Iterable>
    dynamic_array(const Iterable &list) : dynamic_array(index(list.size())) {
        for (const auto & item : list) {
            push_back(item);
        }
    }

    dynamic_array(const dynamic_array<T> &container) : dynamic_array(container.size()) {
        for(index ix = 0; ix < container.size(); ix++)
            this->push_back(container[ix]);
    }

    dynamic_array(dynamic_array<T> && container)  noexcept {
        _data = container._data;
        _current = container._current;
        _cap = container._cap;
        container._data=nullptr;
        container.clear();
    }

    dynamic_array(unsigned capacity = 0) {
        _cap = capacity;
        if(_cap > 0)
            _data = new T[_cap];
    }

    dynamic_array(signed capacity) : dynamic_array{unsigned(capacity)} {
    }

    ~dynamic_array() {
        drain();
    }

    T* data()  {
        return _data;
    }

    const T* data() const {
        return _data;
    }

    dynamic_array<T> & operator=(const dynamic_array<T> &container) {
        if(this!= &container) {
            this->clear();
            for(index ix = 0; ix < container.size(); ix++)
                this->push_back(container[ix]);
        }
        return (*this);
    }

    dynamic_array<T> & operator=(dynamic_array<T> &&container) noexcept {
        if(_data) delete [] _data;
        _data = container._data;
        _current = container._current;
        _cap = container._cap;
        container._data=nullptr;
        container.clear();
        return (*this);
    }

    T& operator[](index i)  {
        return _data[i];
    }

    const T& operator[](index i) const {
        return _data[i];
    }

    const T& peek()  {
        return (*this)[_current];
    }

    void alloc_(bool up) {
        _cap = up ? _cap<<1 : _cap>>1;
        T* _new = nullptr;

        if(_cap==0 && up)
            _cap = 1;

        if(_cap>0) {
            _new = new T[_cap];

            for (index ix = 0; ix < size(); ++ix) {
                _new[ix] = _data[ix];
            }

        }

        if(_data)
            delete [] _data;

        _data = _new;
    }

    int push_back(const T & v)  {
        if(int(_current)>int(_cap-1)) {
            // copy the value, edge case if v belongs
            // to the dynamic array
            const T vv = v;
            alloc_(true);
            _data[_current++] = vv;
        } else {
            _data[_current++] = v;
        }
        return _current-1;
    }

    void push_back(const_dynamic_array_ref container)  {
        const int count = container.size();
        for (int ix = 0; ix < count; ++ix) {
            this->push_back(container[ix]);
        }
    }

    void push_back(dynamic_array_ref container)  {
        const int count = container.size();
        for (int ix = 0; ix < count; ++ix) {
            this->push_back(container[ix]);
        }
    }

    void pop_back()  {
        if(_current < (_cap>>1))
            alloc_(false);
        if(_current==0)
            return;
        _data[_current--].~T();
    }

    void move(index idx)  {
        if(idx < capacity())
            _current = idx;
    }

    void clear()  {
        _current = 0;
    }

    void drain()  {
        delete [] _data;
        _data = nullptr;
        _cap = 0;
        _current = 0;
    }

    T& back() {
        return _data[_current-1];
    }

    bool empty() {
        return _current==0;
    }

    index size() const  {
        return _current;
    }

    index capacity() const  {
        return _cap;
    }

private:
    T *_data = nullptr;
    index _current = 0u;
    index _cap = 0u;
};
