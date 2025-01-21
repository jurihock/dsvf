#pragma once

#include <cmath>
#include <limits>

template<typename Y, typename X>
inline static Y static_clip(const X value)
{
  constexpr auto zero = X(0);
  constexpr auto one  = X(1);

  constexpr auto absmin = std::numeric_limits<X>::epsilon();
  constexpr auto absmax = one;

  static_assert(!(absmin < NAN) || !(NAN == NAN));
  static_assert(  absmax < INFINITY);

  const auto abs = std::abs(value);

  if (value != value)
  {
    return static_cast<Y>(zero);
  }

  if (!std::isgreater(abs, absmin))
  {
    return static_cast<Y>(zero);
  }

  if (std::isgreater(abs, absmax))
  {
    return static_cast<Y>(std::copysign(one, value));
  }

  return static_cast<Y>(value);
}
