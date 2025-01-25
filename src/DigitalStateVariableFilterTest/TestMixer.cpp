#include <DigitalStateVariableFilterTest/Test.h>
#include <DigitalStateVariableFilter/Mixer.h>

TEST_CASE("test mix positive")
{
  for (bool norm : { false, true })
  {
    double sr = 48'000;
    double hz =  1'000;

    double abs = 0 + 1 + 2 + 3 + 4;

    nc::NdArray<double> x = test::wave<double>(sr, hz) / abs;
    nc::NdArray<double> y = nc::zeros_like<double>(x);

    Mixer mixer;
    mixer.normalize(norm);
    mixer.weights(0, 1, 2, 3, 4);

    for (size_t i = 0; i < x.size(); ++i)
    {
      y[i] = mixer.mix(x[i], x[i], x[i], x[i], x[i]); // sum(x * w)
    }

    CHECK(nc::allclose(x, y) == norm);

    if (false)
    {
      x.tofile("x.test");
      y.tofile("y.test");
    }
  }
}

TEST_CASE("test mix negative")
{
  for (bool norm : { false, true })
  {
    double sr = 48'000;
    double hz =  1'000;

    double abs = 0 + 1 + 2 + 3 + 4;

    nc::NdArray<double> x = test::wave<double>(sr, hz) / abs;
    nc::NdArray<double> y = nc::zeros_like<double>(x);

    Mixer mixer;
    mixer.normalize(norm);
    mixer.weights(0, -1, 2, -3, 4);

    for (size_t i = 0; i < x.size(); ++i)
    {
      y[i] = mixer.mix(x[i], x[i], x[i], x[i], x[i]); // sum(x * w)
    }

    CHECK(nc::allclose(x, y) == norm);

    if (false)
    {
      x.tofile("x.test");
      y.tofile("y.test");
    }
  }
}
