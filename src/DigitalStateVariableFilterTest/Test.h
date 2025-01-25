#pragma once

#include <doctest.h>
#include <NumCpp.hpp>

#include <cmath>
#include <limits>
#include <numbers>

namespace test
{
  template<typename T>
  nc::NdArray<T> wave(const double samplerate, const double frequency, const double duration = 1)
  {
    const double pi = std::numbers::pi;
    const double n  = samplerate * duration;

    auto time = nc::arange<double>(n) / samplerate;
    auto wave = nc::sin(2 * pi * frequency * time);

    return wave;
  }
}
