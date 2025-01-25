#include <DigitalStateVariableFilterTest/Test.h>
#include <DigitalStateVariableFilter/Dot.h>

TEST_CASE("test dot")
{
  std::array<double, 5> x = { 0, 1, 2, 3, 4 };
  std::array<double, 5> y = { 4, 3, 2, 1, 0 };

  CHECK(dot(x, y) == (0*4 + 1*3 + 2*2 + 3*1 + 4*0));

  for (double i = 0; i < 5; ++i)
  {
    CHECK(dot(x, i) == (0*i + 1*i + 2*i + 3*i + 4*i));
  }

  CHECK(dot(x) == (0*1 + 1*1 + 2*1 + 3*1 + 4*1));
}
