#pragma once
#include <cstddef>
#include <type_traits>

// Is Array
template <class T>
struct IsArray : std::false_type {};

template <class T, size_t N>
struct IsArray<T[N]> : std::true_type {};

template <class T>
struct IsArray<T[]> : std::true_type {};

template <class T>
constexpr inline bool kIsArrayV = IsArray<T>::value;

// Rank
template <class T>
struct Rank : std::integral_constant<size_t, 0> {};

template <class T>
struct Rank<T[]> : std::integral_constant<size_t, 1 + Rank<T>::value> {};

template <class T, size_t N>
struct Rank<T[N]> : std::integral_constant<size_t, 1 + Rank<T>::value> {};

template <class T>
constexpr inline size_t kRankV = Rank<T>::value;

// Size
template <class T>
struct Size : std::integral_constant<size_t, 1> {};

template <class T>
struct Size<T[]> : std::integral_constant<size_t, 0> {};

template <class T, size_t N>
struct Size<T[N]> : std::integral_constant<size_t, N> {};

template <class T>
constexpr inline size_t kSizeV = Size<T>::value;

// Total Size
template <class T>
struct TotalSize : std::integral_constant<size_t, 1> {};

template <class T, size_t N>
struct TotalSize<T[N]> : std::integral_constant<size_t, N * TotalSize<T>::value> {};

template <class T>
struct TotalSize<T[]> : std::integral_constant<size_t, 0> {};

template <class T>
constexpr inline size_t kTotalSizeV = TotalSize<T>::value;

// Remove Array
template <class T>
struct RemoveArray {
  using Type = T;
};

template <class T, size_t N>
struct RemoveArray<T[N]> {
  using Type = T;
};

template <class T>
struct RemoveArray<T[]> {
  using Type = T;
};

template <class T>
using RemoveArrayT = typename RemoveArray<T>::Type;

// Remove All Arrays
template <class T>
struct RemoveAllArrays {
  using Type = T;
};

template <class T, size_t N>
struct RemoveAllArrays<T[N]> {
  using Type = typename RemoveAllArrays<T>::Type;
};

template <class T>
struct RemoveAllArrays<T[]> {
  using Type = typename RemoveAllArrays<T>::Type;
};

template <class T>
using RemoveAllArraysT = typename RemoveAllArrays<T>::Type;

// Extent
template <class T, size_t Dim>
struct Extent : std::integral_constant<size_t, 0> {};

template <class T, size_t N, size_t Dim>
struct Extent<T[N], Dim> : std::integral_constant<size_t, Extent<T, Dim - 1>::value> {};

template <class T, size_t Dim>
struct Extent<T[], Dim> : std::integral_constant<size_t, Extent<T, Dim - 1>::value> {};

template <class T, size_t N>
struct Extent<T[N], 0> : std::integral_constant<size_t, N> {};

template <class T, size_t Dim>
constexpr inline size_t kExtentV = Extent<T, Dim>::value;
