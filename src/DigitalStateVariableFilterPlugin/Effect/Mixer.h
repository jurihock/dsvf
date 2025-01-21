#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numbers>
#include <numeric>
#include <type_traits>

class Mixer final
{

private:

  template<typename T, typename... Ts>
  using AllSame = std::enable_if_t<std::conjunction_v<std::is_same<T, Ts>...>>;

public:

  template<typename T, typename... Ts, typename = AllSame<T, Ts...>>
  Mixer(const T weight, const Ts... weights) :
    weights({ static_cast<Scalar>(weight), static_cast<Scalar>(weights)... })
  {
    const auto N = std::tuple_size_v<Vector>;
    const auto M = sizeof...(Ts) + 1;

    static_assert(M == N);
  }

  ~Mixer()
  {
  }

  template<typename T, typename... Ts, typename = AllSame<T, Ts...>>
  T mix(const T value, const Ts... values) const
  {
    const auto N = std::tuple_size_v<Vector>;
    const auto M = sizeof...(Ts) + 1;

    static_assert(M == N);

    const Vector& w = weights;
    const Vector  x { value, values... };
    const Scalar  y = std::transform_reduce(w.begin(), w.end(), x.begin(), Zero);

    return static_cast<T>(y);
  }

private:

  using Scalar = double;
  using Vector = std::array<Scalar, 5>;

  static constexpr Scalar Zero = 0;

  Vector weights;

};
