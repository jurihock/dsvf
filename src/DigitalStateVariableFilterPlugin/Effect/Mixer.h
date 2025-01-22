#pragma once

#include <DigitalStateVariableFilterPlugin/Effect/Clip.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <numeric>
#include <type_traits>

class Mixer final
{

private:

  template<typename T, typename... Ts>
  using AllSame = std::enable_if_t<std::conjunction_v<std::is_same<T, Ts>...>>;

  struct Weights
  {
    using Scalar = double;
    using Vector = std::array<Scalar, 5>;

    static constexpr Scalar Zero = 0;

    Vector vector;
  };

public:

  Mixer(const Weights::Scalar weight = 1) : weights({weight})
  {
  }

  ~Mixer()
  {
  }

  template<typename T, typename... Ts, typename = AllSame<T, Ts...>>
  void set(const T weight, const Ts... weights)
  {
    const auto N = std::tuple_size_v<Weights::Vector>;
    const auto M = sizeof...(Ts) + 1;

    static_assert(M == N);

    this->weights.vector =
    {
      static_cast<Weights::Scalar>(weight),
      static_cast<Weights::Scalar>(weights)...
    };
  }

  template<typename T, typename... Ts, typename = AllSame<T, Ts...>>
  T mix(const T value, const Ts... values) const
  {
    const auto N = std::tuple_size_v<Weights::Vector>;
    const auto M = sizeof...(Ts) + 1;

    static_assert(M == N);

    const Weights::Vector& w = weights.vector;
    const Weights::Vector  x { value, values... };
    const Weights::Scalar  y = std::transform_reduce(
      w.begin(), w.end(), x.begin(), Weights::Zero);

    return static_clip<T>(y);
  }

private:

  Weights weights;

};
