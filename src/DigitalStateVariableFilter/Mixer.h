#pragma once

#include <DigitalStateVariableFilter/Clip.h>
#include <DigitalStateVariableFilter/Dot.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numbers>
#include <numeric>
#include <tuple>
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
    static constexpr Scalar One  = 1;
    static constexpr Scalar Eps  = std::numeric_limits<Weights::Scalar>::epsilon();
  };

  struct Config
  {
    bool normalize;
    double normfactor;
    double volume;
    Weights::Vector weights;
  };

public:

  Mixer() :
    config({
      .normalize = false,
      .normfactor = 1,
      .volume = 1,
      .weights = Weights::Vector{1}
    })
  {
  }

  bool normalize() const
  {
    return config.normalize;
  }

  void normalize(bool value)
  {
    config.normalize = value;
  }

  double volume() const
  {
    return config.volume;
  }

  void volume(double value)
  {
    config.volume = value;
  }

  auto weights() const
  {
    return std::tuple_cat(config.weights);
  }

  template<typename T, typename... Ts, typename = AllSame<T, Ts...>>
  void weights(const T value, const Ts... values)
  {
    const auto N = std::tuple_size_v<Weights::Vector>;
    const auto M = sizeof...(Ts) + 1;

    static_assert(N == M);

    config.weights =
    {
      static_cast<Weights::Scalar>(value),
      static_cast<Weights::Scalar>(values)...
    };

    config.normfactor = dot(config.weights);

    if (std::abs(config.normfactor) > Weights::Eps)
    {
      config.normfactor = Weights::One / config.normfactor;
    }
  }

  template<typename T, typename... Ts, typename = AllSame<T, Ts...>>
  T mix(const T value, const Ts... values) const
  {
    const auto N = std::tuple_size_v<Weights::Vector>;
    const auto M = sizeof...(Ts) + 1;

    static_assert(N == M);

    const Weights::Vector& w = config.weights;
    const Weights::Scalar  v = config.volume;
    const Weights::Scalar  f = config.normfactor;

    const Weights::Vector x
    {
      static_cast<Weights::Scalar>(value),
      static_cast<Weights::Scalar>(values)...
    };

    const Weights::Scalar y = dot(w, x);

    return static_clip<T>(config.normalize ? y * v * f : y * v);
  }

private:

  Config config;

};
