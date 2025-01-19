#pragma once

#include <JuceHeader.h>

#include <DigitalStateVariableFilterPlugin/Filter.h>

#include <memory>
#include <span>

class Effect final
{

public:

  Effect(const double samplerate);
  ~Effect();

  int latency() const;

  void frequency(const double value);
  void quality(const double value);

  void dry(const std::span<const float> input, const std::span<float> output);
  void wet(const std::span<const float> input, const std::span<float> output);

protected:

  const double samplerate;

private:

  std::unique_ptr<Filter> filter;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Effect)

};
