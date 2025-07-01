#pragma once
#include <type_traits>
#include <iterator>

template <class Iterator>
typename std::iterator_traits<Iterator>::difference_type Distance(Iterator begin, Iterator end) {
  if constexpr (std::is_base_of_v<std::random_access_iterator_tag,
                                  typename std::iterator_traits<Iterator>::iterator_category>) {
    return end - begin;
  }
  typename std::iterator_traits<Iterator>::difference_type n = 0;
  while (begin != end) {
    ++begin;
    ++n;
  }
  return n;
}

template <class Iterator>
void Advance(Iterator& ptr, int dist) {
  if constexpr (std::is_base_of_v<std::random_access_iterator_tag,
                                  typename std::iterator_traits<Iterator>::iterator_category>) {
    ptr = ptr + dist;
  } else {
    if (dist < 0) {
      if constexpr (std::is_base_of_v<std::bidirectional_iterator_tag,
                                      typename std::iterator_traits<Iterator>::iterator_category>) {
        while (dist != 0) {
          --ptr;
          ++dist;
        }
      }
    } else {
      while (dist != 0) {
        ++ptr;
        --dist;
      }
    }
  }
}

template <class Iterator>
Iterator Next(Iterator ptr, int dist = 1) {
  if constexpr (std::is_base_of_v<std::random_access_iterator_tag,
                                  typename std::iterator_traits<Iterator>::iterator_category>) {
    return ptr + dist;
  }
  if (dist < 0) {
    if constexpr (std::is_base_of_v<std::bidirectional_iterator_tag,
                                    typename std::iterator_traits<Iterator>::iterator_category>) {
      while (dist != 0) {
        --ptr;
        ++dist;
      }
    }
  } else {
    while (dist != 0) {
      ++ptr;
      --dist;
    }
  }
  return ptr;
}

template <class Iterator>
Iterator Prev(Iterator ptr, int dist = 1) {
  if constexpr (std::is_base_of_v<std::random_access_iterator_tag,
                                  typename std::iterator_traits<Iterator>::iterator_category>) {
    return ptr - dist;
  }
  if (dist < 0) {
    while (dist != 0) {
      ++ptr;
      ++dist;
    }
  } else {
    if constexpr (std::is_base_of_v<std::bidirectional_iterator_tag,
                                    typename std::iterator_traits<Iterator>::iterator_category>) {
      while (dist != 0) {
        --ptr;
        --dist;
      }
    }
  }
  return ptr;
}
