#pragma once

#include <JuceHeader.h>

#include <DigitalStateVariableFilter/Filter.h>
#include <DigitalStateVariableFilter/Mixer.h>

#include <memory>
#include <span>

class Effect final
{

public:

  Effect(const double samplerate);

  int latency() const;

  void frequency(const double value);
  void quality(const double value);
  void normalize(const bool value);
  void gain(const double value);
  void volume(const double value);
  void weights(const std::vector<double>& values);

  void dry(const std::span<const float> input, const std::span<float> output);
  void wet(const std::span<const float> input, const std::span<float> output);

private:

  struct
  {
    double samplerate;
    double gain;
  } config;

  std::unique_ptr<Filter> filter;
  std::unique_ptr<Mixer> mixer;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Effect)

};
