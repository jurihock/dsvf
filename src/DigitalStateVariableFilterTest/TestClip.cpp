#include <DigitalStateVariableFilterTest/Test.h>
#include <DigitalStateVariableFilter/Clip.h>

TEST_CASE("test clip")
{
  CHECK(static_clip<double>(NAN) == 0.0);

  CHECK(static_clip<double>(+INFINITY) == +1.0);
  CHECK(static_clip<double>(-INFINITY) == -1.0);

  CHECK(static_clip<double>(+0.1) == +0.1);
  CHECK(static_clip<double>(-0.1) == -0.1);

  CHECK(static_clip<double>(+1.1) == +1.0);
  CHECK(static_clip<double>(-1.1) == -1.0);
}
