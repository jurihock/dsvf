#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <numeric>

class Filter final
{

public:

  struct Config
  {
    double samplerate;
    double frequency;
    double quality;
  };

  struct Coeffs
  {
    std::array<double, 3> c, z;
    std::array<double, 3> hp, bp, lp;
  };

  template<typename T>
  struct State
  {
    T hp, bp, lp;
  };

  Filter(const double samplerate) :
    config({
      .samplerate = samplerate,
      .frequency = samplerate / 4,
      .quality = 1
    })
  {
    sync();
    reset();
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
    std::fill(coeffs.z.begin(), coeffs.z.end(), 0);
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
    const auto y = [](const std::array<double, 3>& v1,
                      const std::array<double, 3>& v2)
    {
      return static_cast<T>(std::transform_reduce(
        v1.begin(), v1.end(), v2.begin(), 0));
    };

    const auto& c = coeffs.c;
    auto& z = coeffs.z;

    z[0] = static_cast<double>(x) - z[1] - z[2];

    State state
    {
      .hp = y(z, coeffs.hp),
      .bp = y(z, coeffs.bp),
      .lp = y(z, coeffs.lp),
    };

    z[2] += c[2] * z[1];
    z[1] += c[1] * z[0];

    return state;
  }

private:

  Config config;
  Coeffs coeffs;

};
