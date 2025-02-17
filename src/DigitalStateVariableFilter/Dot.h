#pragma once

#include <array>
#include <utility>

namespace details
{
  template<typename T, size_t N, size_t... I>
  constexpr T dot(const std::array<T, N>& a, const std::array<T, N>& b, std::index_sequence<I...>)
  {
    return ((a[I] * b[I]) + ...);
  }

  template<typename T, size_t N, size_t... I>
  constexpr T dot(const std::array<T, N>& a, const T b, std::index_sequence<I...>)
  {
    return ((a[I] * b) + ...);
  }

  template<typename T, size_t N, size_t... I>
  constexpr T dot(const std::array<T, N>& a, std::index_sequence<I...>)
  {
    return (a[I] + ...);
  }
}

template<typename T, size_t N, typename I = std::make_index_sequence<N>>
constexpr T dot(const std::array<T, N>& a, const std::array<T, N>& b)
{
  return details::dot<T, N>(a, b, I{});
}

template<typename T, size_t N, typename I = std::make_index_sequence<N>>
constexpr T dot(const std::array<T, N>& a, const T b)
{
  return details::dot<T, N>(a, b, I{});
}

template<typename T, size_t N, typename I = std::make_index_sequence<N>>
constexpr T dot(const std::array<T, N>& a)
{
  return details::dot<T, N>(a, I{});
}
