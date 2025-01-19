#include <DigitalStateVariableFilterPlugin/Editor.h>

#include <DigitalStateVariableFilterPlugin/Logger.h>

Editor::Editor(juce::AudioProcessor& process) :
  GenericAudioProcessorEditor(process),
  process(process)
{
  const int w = 640;
  const int h = 480;

  setSize(w, h);
}

Editor::~Editor()
{
  juce::ignoreUnused(process);
}
