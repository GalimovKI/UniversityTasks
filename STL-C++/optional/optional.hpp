#pragma once

#include <type_traits>
#include <exception>
#include <iostream>

class BadOptionalAccess : public std::runtime_error {
 public:
  BadOptionalAccess() : std::runtime_error("BadOptionalAccess") {
  }
};

template <class T>
using As = std::aligned_storage_t<sizeof(T), alignof(T)>;

template <class T>
class Optional {
  As<T> storage_;
  bool alive_ = false;

 public:
  Optional() = default;
  Optional(const Optional&);  // NOLINT
  Optional(Optional&&);       // NOLINT
  Optional(const T&);         // NOLINT
  Optional(T&&);              // NOLINT
  ~Optional();
  Optional& operator=(const Optional&);
  Optional& operator=(Optional&&);
  Optional& operator=(const T&);
  Optional& operator=(T&&);
  bool HasValue() const;
  explicit operator bool() const;
  T& Value();
  const T& Value() const;
  const T& operator*() const;
  T& operator*();
  template <class... Args>
  T& Emplace(Args&&... args) {
    if (alive_) {
      reinterpret_cast<T*>(&storage_)->~T();
    }
    alive_ = true;
    new (&storage_) T(std::forward<Args>(args)...);
    return *(reinterpret_cast<T*>(&storage_));
  }
  void Reset();
  void Swap(Optional&);
};

template <class T>
bool Optional<T>::HasValue() const {
  return alive_;
}

template <class T>
Optional<T>::Optional(const Optional<T>& other) {
  if (other.alive_) {
    alive_ = true;
    new (&storage_) T(*reinterpret_cast<const T*>(&(other.storage_)));
  }
}

template <class T>
Optional<T>::Optional(Optional<T>&& other) {
  if (other.alive_) {
    alive_ = true;
    new (&storage_) T(std::move(*reinterpret_cast<T*>(&(other.storage_))));
  }
}

template <class T>
Optional<T>::Optional(const T& value) {
  alive_ = true;
  new (&storage_) T(value);
}

template <class T>
Optional<T>::Optional(T&& value) {
  alive_ = true;
  new (&storage_) T(std::move(value));
}

template <class T>
Optional<T>::~Optional() {
  if (alive_) {
    reinterpret_cast<T*>(&storage_)->~T();
  }
}

template <class T>
Optional<T>& Optional<T>::operator=(const Optional<T>& other) {
  if (alive_) {
    reinterpret_cast<T*>(&storage_)->~T();
  }
  if (other.alive_) {
    alive_ = true;
    new (&storage_) T(*reinterpret_cast<const T*>(&(other.storage_)));
  } else {
    alive_ = false;
  }
  return *this;
}

template <class T>
Optional<T>& Optional<T>::operator=(Optional<T>&& other) {
  if (alive_) {
    reinterpret_cast<T*>(&storage_)->~T();
  }
  if (other.alive_) {
    alive_ = true;
    new (&storage_) T(std::move(*reinterpret_cast<T*>(&(other.storage_))));
  } else {
    alive_ = false;
  }
  return *this;
}

template <class T>
Optional<T>& Optional<T>::operator=(const T& value) {
  if (alive_) {
    reinterpret_cast<T*>(&storage_)->~T();
  }
  new (&storage_) T(value);
  alive_ = true;
  return *this;
}

template <class T>
Optional<T>& Optional<T>::operator=(T&& value) {
  if (alive_) {
    reinterpret_cast<T*>(&storage_)->~T();
  }
  new (&storage_) T(std::move(value));
  alive_ = true;
  return *this;
}

template <class T>
Optional<T>::operator bool() const {
  return alive_;
}

template <class T>
T& Optional<T>::Value() {
  if (alive_) {
    return *reinterpret_cast<T*>(&storage_);
  }
  throw BadOptionalAccess{};
}

template <class T>
const T& Optional<T>::Value() const {
  if (alive_) {
    return *reinterpret_cast<const T*>(&storage_);
  }
  throw BadOptionalAccess{};
}

template <class T>
void Optional<T>::Reset() {
  if (alive_) {
    reinterpret_cast<T*>(&storage_)->~T();
  }
  alive_ = false;
}

template <class T>
T& Optional<T>::operator*() {
  return *reinterpret_cast<T*>(&storage_);
}

template <class T>
const T& Optional<T>::operator*() const {
  return *reinterpret_cast<const T*>(&storage_);
}

template <class T>
void Optional<T>::Swap(Optional<T>& other) {
  if (other.alive_ && alive_) {
    std::swap(*reinterpret_cast<T*>(&storage_), *reinterpret_cast<T*>(&(other.storage_)));
    return;
  }
  if (!(other.alive_) && !(alive_)) {
    return;
  }
  if (other.alive_) {
    alive_ = true;
    other.alive_ = false;
    new (&storage_) T(std::move(*reinterpret_cast<T*>(&(other.storage_))));
  } else {
    other.alive_ = true;
    alive_ = false;
    new (&(other.storage_)) T(std::move(*reinterpret_cast<T*>(&storage_)));
  }
}