#include <DigitalStateVariableFilterPlugin/Effect.h>

Effect::Effect(const double samplerate) :
  samplerate(samplerate)
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

void Effect::volume(const double value)
{
  mixer->volume(value);
}

void Effect::weights(const std::vector<double> values)
{
  auto x  = values.at(0);
  auto hp = values.at(1);
  auto bp = values.at(2);
  auto lp = values.at(3);
  auto br = values.at(4);

  mixer->weights(x, hp, bp, lp, br);
}

void Effect::dry(const std::span<const float> input, const std::span<float> output)
{
  std::transform(
    input.begin(),
    input.end(),
    output.begin(),
    [&](const float x)
    {
      filter->filter(x);
      return x;
    });
}

void Effect::wet(const std::span<const float> input, const std::span<float> output)
{
  std::transform(
    input.begin(),
    input.end(),
    output.begin(),
    [&](const float x)
    {
      const auto [hp, bp, lp, br] = filter->filter(x);
      return mixer->mix(x, hp, bp, lp, br);
    });
}
