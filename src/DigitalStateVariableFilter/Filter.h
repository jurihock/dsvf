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

  Filter(const double samplerate = 1, const double frequency = 1, const double quality = 1) :
    config({
      .samplerate = samplerate,
      .frequency = frequency,
      .quality = quality
    })
  {
    reset();
    sync();
  }

  int latency() const
  {
    return 0;
  }

  double samplerate() const
  {
    return config.samplerate;
  }

  void samplerate(const double value)
  {
    config.samplerate = value;
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
    const auto f  = std::clamp<decltype(config.frequency)>(config.frequency, 0, config.samplerate / 2);
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
  auto filter(const T value)
  {
    const auto& c = coeffs.c;
    auto& z = coeffs.z;

    z[0] = static_cast<Coeffs::Scalar>(value) - z[1] - z[2];

    const auto hp = dot(z, coeffs.hp);
    const auto bp = dot(z, coeffs.bp);
    const auto lp = dot(z, coeffs.lp);
    const auto br = hp + lp;

    z[2] += c[2] * z[1];
    z[1] += c[1] * z[0];

    return std::make_tuple(
      value,
      static_clip<T>(hp),
      static_clip<T>(bp),
      static_clip<T>(lp),
      static_clip<T>(br));
  }

private:

  Config config;
  Coeffs coeffs;

};
