#include <DigitalStateVariableFilterPlugin/Plugin.h>

#include <DigitalStateVariableFilterPlugin/Processor.h>

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new Processor();
}
