#pragma once

#define VECTOR_MEMORY_IMPLEMENTED

#include <cstddef>
#include <iterator>
#include <memory>
#include <algorithm>
#include <exception>

namespace detail {
template <class Iterator>
using EnifForwardIt = std::enable_if_t<
    std::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>, int>;
}

template <class T>
class Vector;

template <class T>
bool operator==(const Vector<T>&, const Vector<T>&);

template <class T>
bool operator!=(const Vector<T>&, const Vector<T>&);

template <class T>
bool operator<=(const Vector<T>&, const Vector<T>&);

template <class T>
bool operator>=(const Vector<T>&, const Vector<T>&);

template <class T>
bool operator<(const Vector<T>&, const Vector<T>&);

template <class T>
bool operator>(const Vector<T>&, const Vector<T>&);

template <class T>
class Vector {
 private:
  std::byte* data_;
  size_t size_;
  size_t capacity_;

 public:
  using Iterator = T*;
  using ConstIterator = const T*;
  using ReverseIterator = std::reverse_iterator<T*>;
  using ConstReverseIterator = std::reverse_iterator<const T*>;
  using ValueType = T;
  using Pointer = T*;
  using ConstPointer = const T*;
  using Reference = T&;
  using ConstReference = const T&;
  using SizeType = size_t;

  Vector() noexcept;
  explicit Vector(size_t);
  Vector(size_t, const T&);
  Vector(std::initializer_list<T>);
  template <class ForwardIt, detail::EnifForwardIt<ForwardIt> = 0>
  Vector(ForwardIt, ForwardIt);

  Vector(const Vector<T>&);
  Vector(Vector<T>&&) noexcept;
  Vector& operator=(const Vector<T>&);
  Vector& operator=(Vector<T>&&) noexcept;
  ~Vector();

  size_t Size() const noexcept;
  size_t Capacity() const noexcept;
  bool Empty() const noexcept;

  T& operator[](size_t) noexcept;
  const T& operator[](size_t) const noexcept;
  T& At(size_t);
  const T& At(size_t) const;
  T& Front() noexcept;
  const T& Front() const noexcept;
  T& Back() noexcept;
  const T& Back() const noexcept;
  T* Data() noexcept;
  const T* Data() const noexcept;

  void Swap(Vector<T>&) noexcept;
  void Resize(size_t);
  void Resize(size_t, const T&);
  void Reserve(size_t);
  void ShrinkToFit();
  void Clear() noexcept;
  void PushBack(const T&);
  void PushBack(T&&);
  template <class... Args>
  void EmplaceBack(Args&&... args);
  void PopBack() noexcept;

  Iterator begin() noexcept;                      // NOLINT
  ConstIterator begin() const noexcept;           // NOLINT
  ConstIterator cbegin() const noexcept;          // NOLINT
  Iterator end() noexcept;                        // NOLINT
  ConstIterator end() const noexcept;             // NOLINT
  ConstIterator cend() const noexcept;            // NOLINT
  ReverseIterator rbegin() noexcept;              // NOLINT
  ConstReverseIterator rbegin() const noexcept;   // NOLINT
  ConstReverseIterator crbegin() const noexcept;  // NOLINT
  ReverseIterator rend() noexcept;                // NOLINT
  ConstReverseIterator rend() const noexcept;     // NOLINT
  ConstReverseIterator crend() const noexcept;    // NOLINT

  friend bool operator== <T>(const Vector<T>&, const Vector<T>&);
  friend bool operator!= <T>(const Vector<T>&, const Vector<T>&);
  friend bool operator<= <T>(const Vector<T>&, const Vector<T>&);
  friend bool operator>= <T>(const Vector<T>&, const Vector<T>&);
  friend bool operator< <T>(const Vector<T>&, const Vector<T>&);
  friend bool operator><T>(const Vector<T>&, const Vector<T>&);
};

template <class T>
Vector<T>::Vector() noexcept : data_(nullptr), size_(0), capacity_(size_) {
}

template <class T>
Vector<T>::Vector(size_t n) : data_(nullptr), size_(n), capacity_(n) {
  if (size_) {
    try {
      data_ = new std::byte[capacity_ * sizeof(T)];
      std::uninitialized_default_construct(begin(), end());
    } catch (...) {
      delete[] data_;
      throw;
    }
  }
}

template <class T>
Vector<T>::Vector(size_t n, const T& value) : data_(nullptr), size_(n), capacity_(n) {
  if (size_) {
    try {
      data_ = new std::byte[size_ * sizeof(T)];
      std::uninitialized_fill(begin(), end(), value);
    } catch (...) {
      delete[] data_;
      throw;
    }
  }
}

template <class T>
Vector<T>::Vector(std::initializer_list<T> il) : data_{nullptr}, size_{il.size()}, capacity_{size_} {
  if (size_) {
    try {
      data_ = new std::byte[capacity_ * sizeof(T)];
      std::uninitialized_copy(il.begin(), il.end(), begin());
    } catch (...) {
      delete[] data_;
      throw;
    }
  }
}

template <class T>
template <class ForwardIt, detail::EnifForwardIt<ForwardIt>>
Vector<T>::Vector(ForwardIt start, ForwardIt finish)
    : data_{nullptr}, size_{static_cast<size_t>(std::distance(start, finish))}, capacity_{size_} {
  if (size_) {
    try {
      data_ = new std::byte[size_ * sizeof(T)];
      std::uninitialized_copy(start, finish, begin());
    } catch (...) {
      delete[] data_;
      throw;
    }
  }
}

template <class T>
Vector<T>::Vector(const Vector<T>& other) : data_{nullptr}, size_{other.size_}, capacity_{other.capacity_} {
  if (size_) {
    try {
      data_ = new std::byte[capacity_ * sizeof(T)];
      std::uninitialized_copy(other.begin(), other.end(), begin());
    } catch (...) {
      delete[] data_;
      throw;
    }
  }
}

template <class T>
Vector<T>::Vector(Vector<T>&& other) noexcept : data_{nullptr}, size_{other.size_}, capacity_{other.capacity_} {
  if (size_) {
    data_ = other.data_;
    other.data_ = nullptr;
    other.capacity_ = 0;
    other.size_ = 0;
  }
}

template <class T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other) {
  if (this != &other) {
    auto tmp_size = size_;
    auto tmp_cap = capacity_;
    Clear();
    delete[] data_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    if (size_) {
      try {
        data_ = new std::byte[capacity_ * sizeof(T)];
        std::uninitialized_copy(other.begin(), other.end(), begin());
      } catch (...) {
        delete[] data_;
        data_ = nullptr;
        capacity_ = tmp_cap;
        size_ = tmp_size;
        throw;
      }
    } else {
      data_ = nullptr;
      capacity_ = 0;
      size_ = 0;
    }
  }
  return *this;
}

template <class T>
Vector<T>& Vector<T>::operator=(Vector<T>&& other) noexcept {
  if (this != &other) {
    Clear();
    delete[] data_;
    data_ = std::move(other.data_);
    size_ = std::move(other.size_);
    capacity_ = std::move(other.capacity_);
    other.size_ = 0;
    other.capacity_ = 0;
    other.data_ = nullptr;
  }
  return *this;
}

template <class T>
Vector<T>::~Vector() {
  if (data_) {
    Clear();
  }
  operator delete[](data_);
}

template <class T>
void Vector<T>::Clear() noexcept {
  for (size_t i = 0; i < size_; ++i) {
    std::destroy_at(reinterpret_cast<T*>(data_ + i * sizeof(T)));
  }
  size_ = 0;
}

template <class T>
typename Vector<T>::Iterator Vector<T>::begin() noexcept {
  return reinterpret_cast<T*>(data_);
}

template <class T>
typename Vector<T>::Iterator Vector<T>::end() noexcept {
  return reinterpret_cast<T*>(data_ + size_ * sizeof(T));
}

template <class T>
size_t Vector<T>::Size() const noexcept {
  return size_;
}

template <class T>
size_t Vector<T>::Capacity() const noexcept {
  return capacity_;
}

template <class T>
bool Vector<T>::Empty() const noexcept {
  return (size_ == 0);
}

template <class T>
T& Vector<T>::operator[](size_t i) noexcept {
  return *reinterpret_cast<T*>(data_ + i * sizeof(T));
}

template <class T>
const T& Vector<T>::operator[](size_t i) const noexcept {
  return *reinterpret_cast<const T*>(data_ + i * sizeof(T));
}

template <class T>
T& Vector<T>::At(size_t i) {
  if (i >= size_) {
    throw std::out_of_range("OutOfRange");
  }
  return *reinterpret_cast<T*>(data_ + i * sizeof(T));
}

template <class T>
const T& Vector<T>::At(size_t i) const {
  if (i >= size_) {
    throw std::out_of_range("OutOfRange");
  }
  return *reinterpret_cast<const T*>(data_ + i * sizeof(T));
}

template <class T>
T& Vector<T>::Front() noexcept {
  return *begin();
}

template <class T>
const T& Vector<T>::Front() const noexcept {
  return *cbegin();
}

template <class T>
T& Vector<T>::Back() noexcept {
  return *(end() - 1);
}

template <class T>
const T& Vector<T>::Back() const noexcept {
  return *(cend() - 1);
}

template <class T>
T* Vector<T>::Data() noexcept {
  return begin();
}

template <class T>
const T* Vector<T>::Data() const noexcept {
  return cbegin();
}

template <class T>
void Vector<T>::Swap(Vector<T>& other) noexcept {
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
  std::swap(data_, other.data_);
}

template <class T>
void Vector<T>::Reserve(size_t new_cap) {
  if (new_cap > capacity_) {
    auto tmp_cap = capacity_;
    auto tmp = data_;
    capacity_ = new_cap;
    try {
      data_ = new std::byte[capacity_ * sizeof(T)];
    } catch (...) {
      delete[] data_;
      data_ = tmp;
      capacity_ = tmp_cap;
      throw;
    }
    if (size_) {
      std::uninitialized_move(reinterpret_cast<T*>(tmp), reinterpret_cast<T*>(tmp + size_ * sizeof(T)), begin());
    }
    for (size_t i = 0; i < size_; ++i) {
      std::destroy_at(reinterpret_cast<T*>(tmp + i * sizeof(T)));
    }
    delete[] tmp;
  }
}

template <class T>
void Vector<T>::Resize(size_t n) {
  if (n > size_) {
    if (n > capacity_) {
      auto tmp_cap = capacity_;
      auto tmp = data_;
      auto tmp_size = size_;
      capacity_ = n;
      try {
        data_ = new std::byte[capacity_ * sizeof(T)];
        std::uninitialized_move(reinterpret_cast<T*>(tmp), reinterpret_cast<T*>(tmp + size_ * sizeof(T)), begin());
        std::uninitialized_default_construct(reinterpret_cast<T*>(data_ + size_ * sizeof(T)),
                                             reinterpret_cast<T*>(data_ + n * sizeof(T)));
      } catch (...) {
        Clear();
        delete[] data_;
        data_ = tmp;
        capacity_ = tmp_cap;
        size_ = tmp_size;
        throw;
      }
      for (size_t i = 0; i < size_; ++i) {
        std::destroy_at(reinterpret_cast<T*>(tmp + i * sizeof(T)));
      }
      delete[] tmp;
    } else {
      try {
        std::uninitialized_default_construct(reinterpret_cast<T*>(data_ + size_ * sizeof(T)),
                                             reinterpret_cast<T*>(data_ + n * sizeof(T)));
      } catch (...) {
        throw;
      }
    }
    size_ = n;
  } else {
    if (size_ == n) {
      return;
    }
    if (data_) {
      for (size_t i = n; i < size_; ++i) {
        std::destroy_at(reinterpret_cast<T*>(data_ + i * sizeof(T)));
      }
    }
    size_ = n;
  }
}

template <class T>
void Vector<T>::Resize(size_t n, const T& value) {
  if (n > size_) {
    if (n > capacity_) {
      auto tmp_cap = capacity_;
      auto tmp = data_;
      auto tmp_size = size_;
      capacity_ = n;
      try {
        data_ = new std::byte[capacity_ * sizeof(T)];
        std::uninitialized_move(reinterpret_cast<T*>(tmp), reinterpret_cast<T*>(tmp + size_ * sizeof(T)), begin());
        std::uninitialized_fill(reinterpret_cast<T*>(data_ + size_ * sizeof(T)),
                                reinterpret_cast<T*>(data_ + n * sizeof(T)), value);
      } catch (...) {
        Clear();
        delete[] data_;
        data_ = tmp;
        capacity_ = tmp_cap;
        size_ = tmp_size;
        throw;
      }
      for (size_t i = 0; i < size_; ++i) {
        std::destroy_at(reinterpret_cast<T*>(tmp + i * sizeof(T)));
      }
      delete[] tmp;
    } else {
      try {
        std::uninitialized_fill(reinterpret_cast<T*>(data_ + size_ * sizeof(T)),
                                reinterpret_cast<T*>(data_ + n * sizeof(T)), value);
      } catch (...) {
        throw;
      }
    }
    size_ = n;
  } else {
    if (size_ == n) {
      return;
    }
    if (data_) {
      for (size_t i = n; i < size_; ++i) {
        std::destroy_at(reinterpret_cast<T*>(data_ + i * sizeof(T)));
      }
    }
    size_ = n;
  }
}

template <class T>
void Vector<T>::ShrinkToFit() {
  if (size_ == 0) {
    Clear();
    delete[] data_;
    data_ = nullptr;
    capacity_ = 0;
    return;
  }
  auto tmp_cap = capacity_;
  auto tmp = data_;
  capacity_ = size_;
  try {
    data_ = new std::byte[capacity_ * sizeof(T)];
  } catch (...) {
    delete[] data_;
    data_ = tmp;
    capacity_ = tmp_cap;
    throw;
  }
  std::uninitialized_move(reinterpret_cast<T*>(tmp), reinterpret_cast<T*>(tmp + size_ * sizeof(T)), begin());
  for (size_t i = 0; i < size_; ++i) {
    std::destroy_at(reinterpret_cast<T*>(tmp + i * sizeof(T)));
  }
  delete[] tmp;
}

template <class T>
void Vector<T>::PushBack(const T& value) {
  if (size_ >= capacity_) {
    auto tmp_cap = capacity_;
    auto tmp = data_;
    auto tmp_size = size_;
    capacity_ = size_ == 0 ? 2 : size_ * 2;
    try {
      data_ = new std::byte[capacity_ * sizeof(T)];
      std::uninitialized_move(reinterpret_cast<T*>(tmp), reinterpret_cast<T*>(tmp + size_ * sizeof(T)), begin());
      std::uninitialized_fill(end(), end() + 1, value);
    } catch (...) {
      Clear();
      delete[] data_;
      data_ = tmp;
      capacity_ = tmp_cap;
      size_ = tmp_size;
      throw;
    }
    for (size_t i = 0; i < size_; ++i) {
      std::destroy_at(reinterpret_cast<T*>(tmp + i * sizeof(T)));
    }
    size_++;
    delete[] tmp;
  } else {
    try {
      std::uninitialized_fill(end(), end() + 1, value);
    } catch (...) {
      throw;
    }
    size_++;
  }
}

template <class T>
template <class... Args>
void Vector<T>::EmplaceBack(Args&&... args) {
  if (size_ >= capacity_) {
    auto tmp_cap = capacity_;
    auto tmp = data_;
    capacity_ = size_ == 0 ? 2 : size_ * 2;
    try {
      data_ = new std::byte[capacity_ * sizeof(T)];
      std::uninitialized_move(reinterpret_cast<T*>(tmp), reinterpret_cast<T*>(tmp + size_ * sizeof(T)), begin());
      new (data_ + sizeof(T) * size_) T(std::forward<Args>(args)...);
    } catch (...) {
      delete[] data_;
      data_ = tmp;
      capacity_ = tmp_cap;
      throw;
    }
    for (size_t i = 0; i < size_; ++i) {
      std::destroy_at(reinterpret_cast<T*>(tmp + i * sizeof(T)));
    }
    size_++;
    delete[] tmp;
  } else {
    try {
      new (data_ + sizeof(T) * size_) T(std::forward<Args>(args)...);
      size_++;
    } catch (...) {
      throw;
    }
  }
}

template <class T>
void Vector<T>::PushBack(T&& value) {
  if (size_ >= capacity_) {
    auto tmp_cap = capacity_;
    auto tmp = data_;
    capacity_ = size_ == 0 ? 2 : size_ * 2;
    try {
      data_ = new std::byte[capacity_ * sizeof(T)];
      std::uninitialized_move(reinterpret_cast<T*>(tmp), reinterpret_cast<T*>(tmp + size_ * sizeof(T)), begin());
      new (end()) T(std::move(value));
    } catch (...) {
      delete[] data_;
      data_ = tmp;
      capacity_ = tmp_cap;
      throw;
    }
    for (size_t i = 0; i < size_; ++i) {
      std::destroy_at(reinterpret_cast<T*>(tmp + i * sizeof(T)));
    }
    size_++;
    delete[] tmp;
  } else {
    try {
      new (end()) T(std::move(value));
      size_++;
    } catch (...) {
      throw;
    }
  }
}

template <class T>
void Vector<T>::PopBack() noexcept {
  size_--;
  std::destroy_at(reinterpret_cast<const T*>(data_ + sizeof(T) * size_));
}

template <class T>
bool operator==(const Vector<T>& vec1, const Vector<T>& vec2) {
  if (vec1.Size() == vec2.Size()) {
    for (size_t i = 0; i < vec1.Size(); ++i) {
      if (vec1[i] != vec2[i]) {
        return false;
      }
    }
    return true;
  }
  return false;
}

template <class T>
bool operator!=(const Vector<T>& vec1, const Vector<T>& vec2) {
  return !(vec1 == vec2);
}

template <class T>
bool operator<=(const Vector<T>& vec1, const Vector<T>& vec2) {
  return (vec2 >= vec1);
}

template <class T>
bool operator>=(const Vector<T>& vec1, const Vector<T>& vec2) {
  return !(vec1 < vec2);
}

template <class T>
bool operator<(const Vector<T>& first, const Vector<T>& second) {
  auto size1 = first.Size();
  auto size2 = second.Size();
  size_t i = 0;
  while (i < size1 && i < size2) {
    if (first[i] < second[i]) {
      return true;
    }
    if (first[i] > second[i]) {
      return false;
    }
    ++i;
  }
  return (size1 < size2);
}

template <class T>
bool operator>(const Vector<T>& vec1, const Vector<T>& vec2) {
  return (vec2 < vec1);
}

template <class T>
typename Vector<T>::ConstIterator Vector<T>::begin() const noexcept {
  return reinterpret_cast<const T*>(data_);
}

template <class T>
typename Vector<T>::ConstIterator Vector<T>::cbegin() const noexcept {
  return reinterpret_cast<const T*>(data_);
}

template <class T>
typename Vector<T>::ConstIterator Vector<T>::end() const noexcept {
  return reinterpret_cast<const T*>(data_ + sizeof(T) * size_);
}

template <class T>
typename Vector<T>::ConstIterator Vector<T>::cend() const noexcept {
  return reinterpret_cast<const T*>(data_ + sizeof(T) * size_);
}

template <class T>
typename Vector<T>::ReverseIterator Vector<T>::rbegin() noexcept {
  return ReverseIterator(end());
}

template <class T>
typename Vector<T>::ConstReverseIterator Vector<T>::rbegin() const noexcept {
  return ConstReverseIterator(cend());
}

template <class T>
typename Vector<T>::ConstReverseIterator Vector<T>::crbegin() const noexcept {
  return ConstReverseIterator(cend());
}

template <class T>
typename Vector<T>::ReverseIterator Vector<T>::rend() noexcept {
  return ReverseIterator(begin());
}

template <class T>
typename Vector<T>::ConstReverseIterator Vector<T>::rend() const noexcept {
  return ConstReverseIterator(cbegin());
}

template <class T>
typename Vector<T>::ConstReverseIterator Vector<T>::crend() const noexcept {
  return ConstReverseIterator(cbegin());
}
