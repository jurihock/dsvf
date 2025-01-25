#include <DigitalStateVariableFilterPlugin/Editor.h>

Editor::Editor(juce::AudioProcessor& process) :
  GenericAudioProcessorEditor(process)
{
  const int w = 850 / 1.618, h = 850;

  setSize(w, h);
}
