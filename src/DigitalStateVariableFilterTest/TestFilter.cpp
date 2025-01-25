#include <DigitalStateVariableFilterTest/Test.h>
#include <DigitalStateVariableFilter/Filter.h>

TEST_CASE("test bpf latency")
{
  double sr = 10'000;
  double hz =  1'000;

  nc::NdArray<double> x = test::wave<double>(sr, hz);
  nc::NdArray<double> y = nc::zeros_like<double>(x);
  nc::NdArray<double> z = nc::zeros_like<double>(x);

  nc::NdArray<int> roi = { int(100e-3 * sr), -1 };

  Filter filter(sr, hz);

  for (size_t i = 0; i < x.size(); ++i)
  {
    auto [a, b, c, d, e] = filter.filter(x[i]);

    y[i] = a; // dry
    z[i] = c; // bp
  }

  CHECK(nc::allclose(x[roi], y[roi]));
  CHECK(nc::allclose(x[roi], z[roi]));

  if (false)
  {
    x.tofile("x.test");
    y.tofile("y.test");
    z.tofile("z.test");
  }
}

TEST_CASE("test brf notch")
{
  double sr = 10'000;
  double hz =  1'000;

  nc::NdArray<double> x = test::wave<double>(sr, hz);
  nc::NdArray<double> y = nc::zeros_like<double>(x);
  nc::NdArray<double> z = nc::zeros_like<double>(x);

  nc::NdArray<int> roi = { int(100e-3 * sr), -1 };

  Filter filter(sr, hz);

  for (size_t i = 0; i < x.size(); ++i)
  {
    auto [a, b, c, d, e] = filter.filter(x[i]);

    y[i] = e; // br
  }

  CHECK(nc::allclose(y[roi], z[roi]));

  if (false)
  {
    x.tofile("x.test");
    y.tofile("y.test");
    z.tofile("z.test");
  }
}
