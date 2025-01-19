#include <DigitalStateVariableFilterPlugin/Parameters.h>

#include <DigitalStateVariableFilterPlugin/Logger.h>

Parameters::Parameters(juce::AudioProcessor& process) :
  GenericParameterContainer(process)
{
  add("bypass", new juce::AudioParameterBool(
    { "bypass", schema }, "Bypass", false,
    juce::AudioParameterBoolAttributes()));

  add("frequency", new juce::AudioParameterFloat(
    { "frequency", schema }, "Frequency",
    juce::NormalisableRange<float>(0.0f, 1.0f, 0.1f), 0.0f,
    juce::AudioParameterFloatAttributes().withLabel("hz")));

  add("quality", new juce::AudioParameterFloat(
    { "quality", schema }, "Quality",
    juce::NormalisableRange<float>(0.0f, 1.0f, 0.1f), 0.0f,
    juce::AudioParameterFloatAttributes().withLabel("q")));
}

Parameters::~Parameters()
{
}

void Parameters::onbypass(std::function<void()> callback)
{
  call("bypass", callback);
}

void Parameters::onfrequency(std::function<void()> callback)
{
  call("frequency", callback);
}

void Parameters::onquality(std::function<void()> callback)
{
  call("quality", callback);
}

bool Parameters::bypass() const
{
  return get<bool>("bypass");
}

double Parameters::frequency() const
{
  return get<double>("frequency");
}

double Parameters::quality() const
{
  return get<double>("quality");
}

void Parameters::load(const void* data, const int size)
{
  try
  {
    auto xml = std::unique_ptr<juce::XmlElement>(
      juce::AudioProcessor::getXmlFromBinary(data, size));

    if (xml)
    {
      LOG(xml->toString(juce::XmlElement::TextFormat().withoutHeader()));
    }
    else
    {
      return;
    }

    if (xml->hasTagName(ProjectInfo::projectName) == false) { return; }
    if (xml->getIntAttribute("schema") != schema) { return; }

    read<bool>("bypass", *xml);
    read<double>("frequency", *xml);
    read<double>("quality", *xml);
  }
  catch(const std::exception& exception)
  {
    juce::ignoreUnused(exception);

    LOG(exception.what());
  }
}

void Parameters::save(juce::MemoryBlock& data)
{
  try
  {
    auto xml = std::make_unique<juce::XmlElement>(ProjectInfo::projectName);

    xml->setAttribute("schema", schema);

    write<bool>("bypass", *xml);
    write<double>("frequency", *xml);
    write<double>("quality", *xml);

    LOG(xml->toString(juce::XmlElement::TextFormat().withoutHeader()));

    juce::AudioProcessor::copyXmlToBinary(*xml, data);
  }
  catch(const std::exception& exception)
  {
    juce::ignoreUnused(exception);

    LOG(exception.what());
  }
}
