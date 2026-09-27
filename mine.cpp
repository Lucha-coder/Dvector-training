#include <algorithm>
#include <cstddef>
#include <iostream>
#include <new>
#include <utility>

template <typename T>
class Dvector {
   private:
    size_t sz_;         // размер всего data_
    size_t sz_elem_;    // сколько элементов T
    size_t front_cap_;  // на начало блока данных (с учетом 0)
    T* data_;
    size_t back_cap_;  // на конец с учетом 0 - индексации

    static T* getMemory(size_t);
    static void deleteMemory(T* data, size_t alive, size_t index);

    template <bool IsConst>
    class base_iterator {
       public:
        using pointer_type = std::conditional_t<IsConst, const T*, T*>;
        using reference_type = std::conditional_t<IsConst, const T&, T&>;

       private:
        pointer_type ptr;

       public:
        base_iterator() : ptr(nullptr) {}

        base_iterator(const base_iterator& other) : ptr(other.ptr) {}

        base_iterator(pointer_type ptr) : ptr(ptr) {}

        base_iterator& operator=(const base_iterator& other) {
            ptr = other.ptr;
            return *this;
        }

        bool operator==(const base_iterator& other) const{
            return ptr == other.ptr;
        }

        bool operator!=(const base_iterator& other) const{
            return ptr != other.ptr;
        }

        base_iterator& operator++() {
            ++ptr;
            return *this;
        }

        base_iterator operator++(int) {
            base_iterator copy(ptr);
            ++ptr;
            return copy;
        }

        base_iterator& operator--() {
            --ptr;
            return *this;
        }

        base_iterator operator--(int) {
            base_iterator copy(ptr);
            --ptr;
            return copy;
        }

        reference_type operator*() const{
            return *ptr;
        }

        pointer_type operator->() const {
            return ptr;
        }
    };

   public:
    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return {data_ + front_cap_};
    }

    iterator end() {
        return {data_ + front_cap_ + sz_elem_};
    }

    const_iterator begin() const {
        return {data_ + front_cap_};
    }

    const_iterator end() const {
        return {data_ + front_cap_ + sz_elem_};
    }

    const_iterator cbegin() const {
        return begin();
    }

    const_iterator cend() const {
        return end();
    }

    Dvector();
    Dvector(size_t n);
    Dvector(size_t n, const T& other);
    Dvector(const Dvector& other);
    Dvector(Dvector&&) noexcept;

    Dvector& operator=(const Dvector& other);
    Dvector& operator=(Dvector&& other) noexcept;

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    void reserve(size_t n);
    void reserve(size_t front, size_t back);

    void push_front(const T& obj);
    void push_front(T&& obj);

    void push_back(const T& obj);
    void push_back(T&& obj);

    void pop_back();
    void pop_front();

    size_t size() const;
    size_t front_capacity() const;
    size_t back_capacity() const;

    bool Empty() const;

    T& front();
    const T& front() const;
    T& back();
    const T& back() const;
    ~Dvector();
};

template <typename T>
T* Dvector<T>::getMemory(size_t n) {
    if (n == 0) {
        return nullptr;
    }
    return reinterpret_cast<T*>(new std::byte[n * sizeof(T)]);
}

template <typename T>
void Dvector<T>::deleteMemory(T* data, size_t alive, size_t index) {
    for (size_t i = 0; i < alive; ++i) {
        data[index + i].~T();
    }
    delete[] reinterpret_cast<std::byte*>(data);
}

template <typename T>
Dvector<T>::Dvector() : sz_(0), sz_elem_(0), front_cap_(0), data_(getMemory(0)), back_cap_(0) {}

template <typename T>
Dvector<T>::Dvector(size_t n)
    : sz_(n), sz_elem_(n), front_cap_(0), data_(getMemory(n)), back_cap_(n - 1) {
    if (n == 0) {
        back_cap_ = 0;
        return;
    }
    size_t i = front_cap_;
    try {
        for (; i <= back_cap_; ++i) {
            new (data_ + i) T();
        }
    } catch (...) {
        deleteMemory(data_, i, front_cap_);
        sz_ = 0;
        sz_elem_ = 0;
        front_cap_ = 0;
        back_cap_ = 0;
        throw;
    }
}

template <typename T>
Dvector<T>::Dvector(size_t n, const T& other)
    : sz_(n), sz_elem_(n), front_cap_(0), data_(getMemory(n)), back_cap_(n - 1) {
    if (n == 0) {
        back_cap_ = 0;
        return;
    }
    size_t i = front_cap_;
    try {
        for (; i <= back_cap_; ++i) {
            new (data_ + i) T(other);
        }
    } catch (...) {
        deleteMemory(data_, i, front_cap_);
        sz_ = 0;
        sz_elem_ = 0;
        front_cap_ = 0;
        back_cap_ = 0;
        throw;
    }
}

template <typename T>
Dvector<T>::Dvector(const Dvector& other)
    : sz_(other.sz_elem_),
      sz_elem_(other.sz_elem_),
      front_cap_(0),
      data_(getMemory(other.sz_elem_)),
      back_cap_(sz_ - 1) {
    if (other.sz_elem_ == 0) {
        back_cap_ = 0;
        return;
    }

    size_t i = front_cap_;  // 0
    try {
        for (; i <= back_cap_; ++i) {
            new (data_ + i) T(other.data_[other.front_cap_ + i]);
        }
    } catch (...) {
        deleteMemory(data_, i, front_cap_);
        sz_ = 0;
        sz_elem_ = 0;
        front_cap_ = 0;
        back_cap_ = 0;
        throw;
    }
}

template <typename T>
Dvector<T>::Dvector(Dvector&& other) noexcept
    : sz_(other.sz_),
      sz_elem_(other.sz_elem_),
      front_cap_(other.front_cap_),
      data_(other.data_),
      back_cap_(other.back_cap_) {
    other.data_ = getMemory(0);
    other.sz_ = 0;
    other.sz_elem_ = 0;
    other.front_cap_ = 0;
    other.back_cap_ = 0;
}

template <typename T>
Dvector<T>& Dvector<T>::operator=(const Dvector& other) {
    if (other.Empty()) {
        deleteMemory(data_, sz_elem_, front_cap_);
        data_ = nullptr;
        sz_ = 0;
        sz_elem_ = 0;
        front_cap_ = 0;
        back_cap_ = 0;
        return *this;
    }

    T* newData = getMemory(other.sz_elem_);

    size_t i = other.front_cap_;
    try {
        for (; i <= other.back_cap_; ++i) {
            new (newData + i - other.front_cap_) T(other.data_[i]);
        }
    } catch (...) {
        deleteMemory(newData, i - other.front_cap_, 0);
        throw;
    }

    deleteMemory(data_, sz_elem_, front_cap_);
    data_ = newData;
    sz_ = other.sz_elem_;
    sz_elem_ = other.sz_elem_;
    front_cap_ = 0;
    back_cap_ = sz_ - 1;

    return *this;
}

template <typename T>
Dvector<T>& Dvector<T>::operator=(Dvector&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    deleteMemory(data_, sz_elem_, front_cap_);

    sz_ = other.sz_;
    sz_elem_ = other.sz_elem_;
    front_cap_ = other.front_cap_;
    data_ = other.data_;
    back_cap_ = other.back_cap_;

    other.data_ = getMemory(0);
    other.sz_ = 0;
    other.sz_elem_ = 0;
    other.front_cap_ = 0;
    other.back_cap_ = 0;

    return *this;
}

template <typename T>
T& Dvector<T>::operator[](size_t index) {
    return data_[front_cap_ + index];
}

template <typename T>
const T& Dvector<T>::operator[](size_t index) const {
    return data_[front_cap_ + index];
}

template <typename T>
void Dvector<T>::reserve(size_t n) {
    if (n <= sz_) {
        return;
    }

    T* newData = getMemory(n);
    size_t i = 0;
    size_t newFront = (n - sz_elem_) / 2;
    try {
        for (; i < sz_elem_; ++i) {
            new (newData + newFront + i) T(std::move_if_noexcept(data_[front_cap_ + i]));
        }
    } catch (...) {
        deleteMemory(newData, i, newFront);
        throw;
    }
    deleteMemory(data_, sz_elem_, front_cap_);
    data_ = newData;
    sz_ = n;
    front_cap_ = newFront;
    back_cap_ = (sz_elem_ == 0) ? 0 : (front_cap_ + sz_elem_ - 1);
}

template <typename T>
void Dvector<T>::reserve(size_t front, size_t back) {
    if (front_cap_ >= front && (sz_ - front_cap_ - sz_elem_) >= back) {
        return;
    }
    size_t needfront = std::max(front_cap_, front);
    size_t needback = std::max((sz_ - front_cap_ - sz_elem_), back);

    T* newData = getMemory(needfront + sz_elem_ + needback);
    size_t i = 0;

    try {
        for (; i < sz_elem_; ++i) {
            new (newData + needfront + i) T(std::move_if_noexcept(data_[front_cap_ + i]));
        }
    } catch (...) {
        deleteMemory(newData, i, needfront);
        throw;
    }

    deleteMemory(data_, sz_elem_, front_cap_);

    data_ = newData;
    sz_ = needfront + sz_elem_ + needback;
    front_cap_ = needfront;
    back_cap_ = (sz_elem_ == 0) ? 0 : (needfront + sz_elem_ - 1);
}

template <typename T>
void Dvector<T>::push_back(const T& obj) {
    if (sz_elem_ == 0) {
        reserve(1);
        new (data_ + front_cap_) T(obj);

        back_cap_ = front_cap_;
        sz_elem_ = 1;

        return;
    }

    if ((sz_ - front_cap_ - sz_elem_) == 0) {
        reserve(0, 1);
    }
    new (data_ + back_cap_ + 1) T(obj);
    ++sz_elem_;
    ++back_cap_;
}

template <typename T>
void Dvector<T>::push_back(T&& obj) {
    if (sz_elem_ == 0) {
        if (sz_ == 0) {
            reserve(1);
        }

        new (data_ + front_cap_) T(std::move(obj));

        back_cap_ = front_cap_;
        sz_elem_ = 1;
        return;
    }

    if (sz_ - front_cap_ - sz_elem_ == 0) {
        reserve(0, 1);
    }

    new (data_ + back_cap_ + 1) T(std::move(obj));

    ++sz_elem_;
    ++back_cap_;
}

template <typename T>
void Dvector<T>::push_front(const T& obj) {
    if (sz_elem_ == 0) {
        if (sz_ == 0) {
            reserve(1);
        }

        new (data_ + front_cap_) T(obj);

        back_cap_ = front_cap_;
        sz_elem_ = 1;

        return;
    }

    if (front_cap_ == 0) {
        reserve(1, 0);
    }

    new (data_ + front_cap_ - 1) T(obj);

    --front_cap_;
    ++sz_elem_;
}

template <typename T>
void Dvector<T>::push_front(T&& obj) {
    if (sz_elem_ == 0) {
        if (sz_ == 0) {
            reserve(1);
        }

        new (data_ + front_cap_) T(std::move(obj));

        back_cap_ = front_cap_;
        sz_elem_ = 1;
        return;
    }

    if (front_cap_ == 0) {
        reserve(1, 0);
    }

    new (data_ + front_cap_ - 1) T(std::move(obj));

    --front_cap_;
    ++sz_elem_;
}

template <typename T>
void Dvector<T>::pop_back() {
    if (sz_elem_ == 0) {
        return;
    }

    data_[back_cap_].~T();
    --sz_elem_;

    if (sz_elem_ == 0) {
        front_cap_ = 0;
        back_cap_ = 0;
        return;
    }

    --back_cap_;
}

template <typename T>
void Dvector<T>::pop_front() {
    if (sz_elem_ == 0) {
        return;
    }

    data_[front_cap_].~T();
    --sz_elem_;

    if (sz_elem_ == 0) {
        front_cap_ = 0;
        back_cap_ = 0;
        return;
    }

    ++front_cap_;
}

template <typename T>
size_t Dvector<T>::size() const {
    return sz_elem_;
}

template <typename T>
size_t Dvector<T>::front_capacity() const {
    return front_cap_;
}

template <typename T>
size_t Dvector<T>::back_capacity() const {
    return (sz_ - front_cap_ - sz_elem_);
}

template <typename T>
bool Dvector<T>::Empty() const {
    return sz_elem_ == 0;
}

template<typename T>
T& Dvector<T>::front() {
    return data_[front_cap_];
}

template <typename T>
const T& Dvector<T>::front() const {
    return data_[front_cap_];
}

template <typename T>
T& Dvector<T>::back() {
    return data_[front_cap_ + sz_elem_ - 1];
}

template <typename T>
const T& Dvector<T>::back() const {
    return data_[front_cap_ + sz_elem_ - 1];
}

template <typename T>
Dvector<T>::~Dvector() {
    deleteMemory(data_, sz_elem_, front_cap_);
    sz_ = 0;
    sz_elem_ = 0;
    front_cap_ = 0;
    data_ = nullptr;
    back_cap_ = 0;
}
