#include <DigitalStateVariableFilterPlugin/Editor.h>

Editor::Editor(juce::AudioProcessor& process) :
  GenericAudioProcessorEditor(process)
{
  const int w = 640, h = 480;

  setSize(w, h);
}
