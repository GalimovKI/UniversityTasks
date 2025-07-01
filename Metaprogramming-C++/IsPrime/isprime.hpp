#pragma once

#include <type_traits>
#include <cstddef>

template <size_t Left, size_t Right, size_t N, size_t Mid = (Left + Right) / 2>
struct Sqrt : std::integral_constant<size_t, std::conditional_t<(Mid * Mid <= N && (Mid < 1'000'000'000)),
                                                                Sqrt<Mid, Right, N>, Sqrt<Left, Mid, N>>::value> {};

template <size_t Result, size_t N>
struct Sqrt<Result, Result + 1, N> : std::integral_constant<size_t, Result> {};

template <>
struct Sqrt<0, 1, 0> : std::integral_constant<size_t, 0> {};

template <size_t N>
constexpr inline size_t kSqrtV = Sqrt<0, N + 2, N>::value;

template <size_t N, size_t Left, size_t Right>
struct HasDivisorOn : std::bool_constant<HasDivisorOn<N, Left, (Left + Right) / 2 - 1>::value ||
                                         HasDivisorOn<N, (Right + Left) / 2, Right>::value> {};

template <>
struct HasDivisorOn<1, 2, 1> : std::true_type {};

template <>
struct HasDivisorOn<2, 2, 1> : std::false_type {};

template <>
struct HasDivisorOn<3, 2, 1> : std::false_type {};

template <size_t N, size_t Value>
struct HasDivisorOn<N, Value, Value> : std::bool_constant<(N % Value == 0)> {};

template <size_t N, size_t Value>
struct HasDivisorOn<N, Value, Value + 1> : std::bool_constant<(N % Value == 0) || (N % (Value + 1) == 0)> {};

template <size_t N, size_t L, size_t R>
constexpr inline bool kHasDivisorOnV = HasDivisorOn<N, L, R>::value;

template <size_t N>
constexpr inline bool kIsPrimeV = !kHasDivisorOnV<N, 2, kSqrtV<N>>;
