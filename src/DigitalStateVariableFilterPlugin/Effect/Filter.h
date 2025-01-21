#pragma once

#include <DigitalStateVariableFilterPlugin/Effect/Clip.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <numeric>
#include <type_traits>

class Filter final
{

private:

  struct Config
  {
    double samplerate;
    double frequency;
    double quality;
  };

  struct Coeffs
  {
    using Scalar = double;
    using Vector = std::array<Scalar, 3>;

    static constexpr Scalar Zero = 0;

    Vector c, z;
    Vector hp, bp, lp;
  };

public:

  template<typename T>
  struct State
  {
    T hp, bp, lp, br;
  };

  Filter(const double samplerate) :
    config({
      .samplerate = samplerate,
      .frequency = samplerate / 4,
      .quality = 1
    })
  {
    reset();
    sync();
  }

  ~Filter()
  {
  }

  int latency() const
  {
    return 0;
  }

  double samplerate() const
  {
    return config.samplerate;
  }

  double frequency() const
  {
    return config.frequency;
  }

  void frequency(const double value)
  {
    config.frequency = value;
  }

  double quality() const
  {
    return config.quality;
  }

  void quality(const double value)
  {
    config.quality = value;
  }

  void reset()
  {
    std::fill(
      coeffs.z.begin(),
      coeffs.z.end(),
      Coeffs::Zero);
  }

  void sync()
  {
    const auto pi = std::numbers::pi;
    const auto sr = config.samplerate;
    const auto f  = config.frequency;
    const auto q  = config.quality;

    const auto w = 2 * std::tan(pi * f / sr);
    const auto a = w / q;
    const auto b = w * w;

    const auto c1 = (a + b) / (1 + a / 2 + b / 4);
    const auto c2 = b / (a + b);

    coeffs.c  = { 0, c1, c2 };
    coeffs.hp = { 1 - c1 / 2 + c1 * c2 / 4, 0, 0 };
    coeffs.bp = { (1 - c2) * c1 / 2, 1 - c2, 0 };
    coeffs.lp = { c1 * c2 / 4, c2, 1 };
  }

  template<typename T>
  State<T> filter(const T x)
  {
    const auto y = [](const Coeffs::Vector& a,
                      const Coeffs::Vector& b)
    {
      return std::transform_reduce(
        a.begin(),
        a.end(),
        b.begin(),
        Coeffs::Zero);
    };

    const auto& c = coeffs.c;
    auto& z = coeffs.z;

    z[0] = static_cast<Coeffs::Scalar>(x) - z[1] - z[2];

    const auto hp = y(z, coeffs.hp);
    const auto bp = y(z, coeffs.bp);
    const auto lp = y(z, coeffs.lp);
    const auto br = hp + lp;

    State<T> state
    {
      .hp = static_clip<T>(hp),
      .bp = static_clip<T>(bp),
      .lp = static_clip<T>(lp),
      .br = static_clip<T>(br),
    };

    z[2] += c[2] * z[1];
    z[1] += c[1] * z[0];

    return state;
  }

private:

  Config config;
  Coeffs coeffs;

};
