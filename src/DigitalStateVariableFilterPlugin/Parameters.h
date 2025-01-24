#pragma once

#include <JuceHeader.h>

#include <DigitalStateVariableFilterPlugin/Utils/Parameters/GenericParameterContainer.h>

class Parameters final : public GenericParameterContainer
{

public:

  Parameters(juce::AudioProcessor& process);

  void onbypass(std::function<void()> callback);
  void onnormalize(std::function<void()> callback);
  void onfrequency(std::function<void()> callback);
  void onquality(std::function<void()> callback);
  void onweights(std::function<void()> callback);
  void ongain(std::function<void()> callback);
  void onvolume(std::function<void()> callback);

  bool bypass() const;
  bool normalize() const;
  double frequency() const;
  double quality() const;
  std::vector<double> weights() const;
  double gain() const;
  double volume() const;

  void load(const void* data, const int size);
  void save(juce::MemoryBlock& data);

private:

  const int schema = 1;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)

};
