#include <DigitalStateVariableFilterPlugin/Parameters.h>

#include <DigitalStateVariableFilterPlugin/Logger.h>

Parameters::Parameters(juce::AudioProcessor& process) :
  GenericParameterContainer(process)
{
  const auto dB = [](float x, int)
  {
    const float e = std::numeric_limits<float>::epsilon();
    const float i = std::numeric_limits<float>::infinity();
    const float y = (x > e) ? 20 * std::log10(x) : -i;

    return std::format("{:.0f} ({:.1f})", y, x);
  };

  add("bypass", new juce::AudioParameterBool(
    { "bypass", schema }, "Bypass", false,
    juce::AudioParameterBoolAttributes()));

  add("frequency", new juce::AudioParameterFloat(
    { "frequency", schema }, "Frequency",
    juce::NormalisableRange<float>(20.0f, 20'000.0f, 10.0f), 1'000.0f,
    juce::AudioParameterFloatAttributes()
      .withLabel("hz")));

  add("quality", new juce::AudioParameterFloat(
    { "quality", schema }, "Quality",
    juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f), 1.0f,
    juce::AudioParameterFloatAttributes()
      .withLabel("q")));

  add("mix", new juce::AudioParameterFloat(
    { "dry", schema }, "Dry level",
    juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 1.0f,
    juce::AudioParameterFloatAttributes()
      .withLabel("dB")
      .withStringFromValueFunction(dB)));

  add("mix", new juce::AudioParameterFloat(
    { "wet.hp", schema }, "HP level",
    juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 0.0f,
    juce::AudioParameterFloatAttributes()
      .withLabel("dB")));

  add("mix", new juce::AudioParameterFloat(
    { "wet.lp", schema }, "LP level",
    juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 0.0f,
    juce::AudioParameterFloatAttributes()
      .withLabel("dB")));

  add("mix", new juce::AudioParameterFloat(
    { "wet.bp", schema }, "BP level",
    juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 0.0f,
    juce::AudioParameterFloatAttributes()
      .withLabel("dB")));

  add("mix", new juce::AudioParameterFloat(
    { "wet.br", schema }, "BR level",
    juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 0.0f,
    juce::AudioParameterFloatAttributes()
      .withLabel("dB")));
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

void Parameters::onmix(std::function<void()> callback)
{
  call("mix", callback);
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

std::vector<double> Parameters::weights() const
{
  return
  {
    get<double>("dry"),
    get<double>("wet.hp"),
    get<double>("wet.bp"),
    get<double>("wet.lp"),
    get<double>("wet.br"),
  };
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
    read<double>("dry", *xml);
    read<double>("wet.hp", *xml);
    read<double>("wet.lp", *xml);
    read<double>("wet.bp", *xml);
    read<double>("wet.br", *xml);
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
    write<double>("dry", *xml);
    write<double>("wet.hp", *xml);
    write<double>("wet.lp", *xml);
    write<double>("wet.bp", *xml);
    write<double>("wet.br", *xml);

    LOG(xml->toString(juce::XmlElement::TextFormat().withoutHeader()));

    juce::AudioProcessor::copyXmlToBinary(*xml, data);
  }
  catch(const std::exception& exception)
  {
    juce::ignoreUnused(exception);

    LOG(exception.what());
  }
}
