#include <iostream>

template <typename T>
class Devector {
private:
  T* arr_ = nullptr;
  T* arr_real_ = nullptr;
  size_t size_ = 0;
  size_t front_capacity_ = 0;
  size_t back_capacity_ = 0;
public:
  template<bool IsConst>
  struct base_iterator {
    using type = std::conditional_t<IsConst, const T, T>;
    using reference = type&;
    using pointer = type*;
    using different_types = std::ptrdiff_t;

    T* ptr_;

    base_iterator() : ptr_(nullptr) {}
    base_iterator(T* other_ptr) : ptr_(other_ptr) {}

    base_iterator& operator+=(different_types n) & {
      ptr_ += n;
      return *this;
    }
    base_iterator& operator-=(different_types n) & {
      ptr_ -= n;
      return *this;
    }
    base_iterator operator+(different_types n) const {
      base_iterator copy(ptr_);
      copy += n;
      return copy;
    }
    base_iterator operator+(base_iterator other) const {
      return ptr_ + other.ptr_;
    }
    base_iterator operator-(different_types n) const {
      base_iterator copy(ptr_);
      copy -= n;
      return copy;
    }
    base_iterator operator++() {
      ptr_ += 1;
      return *this;
    }
    base_iterator operator--() {
      ptr_ -= 1;
      return *this;
    }
    base_iterator operator++(int) const {
      base_iterator copy(ptr_);
      ptr_ += 1;
      return copy;
    }
    base_iterator operator--(int) const {
      base_iterator copy(ptr_);
      ptr_ -= 1;
      return copy;
    }
    reference operator*() {
      return *ptr_;
    }
    pointer operator->() {
      return ptr_;
    }
    bool operator==(base_iterator other) const {
      return ptr_ == other.ptr_;
    }
    bool operator!=(base_iterator other) const {
      return !(ptr_ == other.ptr_);
    }
  };
  
  using Iterator = base_iterator<false>;
  using ConstIterator = base_iterator<true>;

  Iterator begin() {
    return Iterator(arr_real_);
  }
  ConstIterator begin() const {
    return ConstIterator(arr_real_);
  }
  ConstIterator cbegin() {
    return ConstIterator(arr_real_);
  }
  Iterator end() {
    return Iterator(arr_real_ + size_);
  }
  ConstIterator end() const {
    return ConstIterator(arr_real_ + size_);
  }
  ConstIterator cend() {
    return ConstIterator(arr_real_ + size_);
  }

  size_t size() const {
    return size_;
  }
  size_t front_capacity() const {
    return front_capacity_;
  }
  size_t back_capacity() const {
    return back_capacity_;
  }
  T& operator[](size_t ind) {
    return arr_[ind];
  }
  const T& operator[](size_t ind) const {
    return arr_[ind];
  }
  T& front() {
    return *begin();
  }
  const T& front() const {
    return *begin();
  }
  T& back() {
    return *(end() - 1);
  }
  const T& back() const {
    return *(end() - 1);
  }

  // ==================================================================
  // !!!                         reserve                            !!!
  // ==================================================================
  void reserve(size_t front, size_t back) {
    if (front_capacity_ >= front && back_capacity_ >= back) {
      return;
    }
    T* new_arr = new T[front + size_ + back];
    T* new_arr_real = new_arr + front;
    for (auto i = 0; i < size_; ++i) {
      new_arr[front + i] = arr_real_[i];
    }
    delete [] arr_;
    arr_ = new_arr;
    arr_real_ = new_arr_real;
    front_capacity_ = front;
    back_capacity_ = back;
  }

  void reserve(size_t n) {
    reserve(0, n);
  }

  // ===============================================================
  // !!!               reserve   back_capacity_                  !!!
  // ===============================================================
  template<typename ... Args>
  void emplace_back(Args&& ... args) {
    if (back_capacity_ == 0) {
      reserve(front_capacity_, (back_capacity_ == 0 ? 1 : back_capacity_ * 2));
    }
    new (arr_real_ + size_) T(std::forward<Args>(args)...);
    size_++;
    back_capacity_--;
  }

  void push_back(const T& val) {
    emplace_back(val);
  }
  void push_back(T&& val) {
    emplace_back(std::move(val));
  }

  // ===============================================================
  // !!!               reserve   front_capacity_                 !!!
  // ===============================================================
  template<typename ... Args>
  void emplace_front(Args&& ... args) {
    if (front_capacity_ == 0) {
      reserve((front_capacity_ == 0 ? 1 : front_capacity_ * 2), back_capacity_);
    }
    new (arr_real_ - 1) T(std::forward<Args>(args)...);
    arr_real_ = arr_real_ - 1;
    size_++;
    front_capacity_--;
  }

  void push_front(const T& val) {
    emplace_front(val);
  }
  void push_front(T&& val) {
    emplace_front(std::move(val));
  }
  void pop_back() {
    (end() - 1)->~T();
    size_--;
    back_capacity_++;
  }
  void pop_front() {
    begin()->~T();
    arr_real_ += 1;
    size_--;
    front_capacity_++;
  }

  Devector() {}
  Devector(size_t n) {
    for (size_t i = 0; i < n; ++i) {
      push_back(T());
    }
  }
  Devector(size_t n, const T& object) {
    for (size_t i = 0; i < n; ++i) {
      push_back(object);
    }
  }
  Devector(const Devector& other) {
    size_t sz = other.size();
    for (size_t i = 0; i < sz; ++i) {
      push_back(other[i]);
    }
  }

  void swap(Devector& other) {
    std::swap(arr_, other.arr_);
    std::swap(arr_real_, other.arr_real_);
    std::swap(size_, other.size_);
    std::swap(front_capacity_, other.front_capacity_);
    std::swap(back_capacity_, other.back_capacity_);
  }
  Devector(Devector&& other) {
    swap(other);
  }
  Devector& operator=(const Devector& other) {
    Devector copy(other);
    swap(copy);
    return *this;
  }
  Devector& operator=(Devector&& other) {
    swap(other);
    other.arr_ = nullptr;
    other.arr_real_ = nullptr;
    other.size_ = 0;
    other.front_capacity_ = 0;
    other.back_capacity_ = 0;
    return *this;
  }

  ~Devector() {
    delete [] arr_;
  }
};


int main() {
  Devector<int> a(3);
  for (auto i : a) std::cout << i << " ";
  std::cout << "<\n<\n";

  Devector<int> as(8, 13);
  for (auto i : as) std::cout << i << " ";
  std::cout << "<\n";
  as.push_back(15);
  as.push_front(-12);
  for (auto i : as) std::cout << i << " ";
  std::cout << "<\n";
  as.pop_back();
  as.pop_back();
  as.pop_back();
  for (auto i : as) std::cout << i << " ";
  std::cout << "<\n";
  as.pop_front();
  for (auto i : as) std::cout << i << " ";
  std::cout << "<\n";
  as = a;
  for (auto i : as) std::cout << i << " ";
  std::cout << "<\n";
  as.push_back(1984);
  as.push_front(-834756);
  for (auto i : as) std::cout << i << " ";
  std::cout << "<\n";
  as = std::move(a);
  for (auto i : as) std::cout << i << " ";
  std::cout << "<\n";
  for (auto i : a) std::cout << i << " ";
  std::cout << "<\n";
  a.push_front(1);
  for (auto i : a) std::cout << i << " ";
  std::cout << "<\n\n";
  
  Devector<int> ass(std::move(a));
  for (auto i : ass) std::cout << i << " ";
  std::cout << " <\n";
  for (auto i : a) std::cout << i << " ";
  std::cout << "<\n";
  a.push_front(1);
  a.push_back(0);
  for (auto i : a) std::cout << i << " ";
  std::cout << "<\n";
}
