#include <DigitalStateVariableFilterPlugin/Effect.h>

Effect::Effect(const double samplerate) :
  config({
    .samplerate = samplerate,
    .gain = 1
  })
{
  filter = std::make_unique<Filter>(samplerate);
  mixer = std::make_unique<Mixer>();
}

int Effect::latency() const
{
  return filter->latency();
}

void Effect::frequency(const double value)
{
  filter->frequency(value);
  filter->sync();
}

void Effect::quality(const double value)
{
  filter->quality(value);
  filter->sync();
}

void Effect::normalize(const bool value)
{
  mixer->normalize(value);
}

void Effect::gain(const double value)
{
  config.gain = value;
}

void Effect::volume(const double value)
{
  mixer->volume(value);
}

void Effect::weights(const std::vector<double>& values)
{
  const auto pack = [](auto arr, auto... idx)
  {
    return std::make_tuple(arr.at(idx)...);
  };

  const auto [a, b, c, d, e] = pack(values, 0, 1, 2, 3, 4);

  mixer->weights(a, b, c, d, e);
}

void Effect::dry(const std::span<const float> input, const std::span<float> output)
{
  std::transform(
    input.begin(),
    input.end(),
    output.begin(),
    [&](const auto x)
    {
      const auto [a, b, c, d, e] = filter->filter(x);

      return a;

      static_assert(std::is_same_v<decltype(x), decltype(a)>);
    });
}

void Effect::wet(const std::span<const float> input, const std::span<float> output)
{
  std::transform(
    input.begin(),
    input.end(),
    output.begin(),
    [&](const auto x)
    {
      const auto y = static_cast<decltype(x)>(config.gain);
      const auto [a, b, c, d, e] = filter->filter(x * y);

      return mixer->mix(a, b, c, d, e);

      static_assert(std::is_same_v<decltype(x), decltype(a)>);
    });
}
