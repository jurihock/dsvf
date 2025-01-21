#include <DigitalStateVariableFilterPlugin/Effect.h>

Effect::Effect(const double samplerate) :
  samplerate(samplerate)
{
  filter = std::make_unique<Filter>(samplerate);
  mixer = std::make_unique<Mixer>(1, 1, 1, 1, 1);
}

Effect::~Effect()
{
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
      const auto& [hp, bp, lp, br] = filter->filter(x);
      return mixer->mix(x, hp, bp, lp, br);
    });
}
