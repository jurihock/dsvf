#include <DigitalStateVariableFilterPlugin/Parameters.h>

#include <DigitalStateVariableFilterPlugin/Utils/Logger.h>

Parameters::Parameters(juce::AudioProcessor& process) :
  GenericParameterContainer(process)
{
  const auto fmt2int = [](float val, int)
  {
    return juce::String::formatted("%.0f", val);
  };

  const auto fmt2flt = [](float val, int)
  {
    return juce::String::formatted("%.2f", val);
  };

  const auto fmt2log = [](float val, int)
  {
    constexpr float eps = std::numeric_limits<float>::epsilon();
    constexpr float inf = std::numeric_limits<float>::infinity();

    const float log = (std::abs(val) < eps) ? -inf : 20 * std::log10(val);

    return juce::String::formatted("%.0f (%.2f)", log, val);
  };

  const auto norm2log = [](float min, float max, float val) -> float
  {
    constexpr float eps = std::numeric_limits<float>::epsilon();

    if (std::abs(val) < eps)
    {
      return 0;
    }

    min = std::max(min, 0.1f);

    min = std::log(min);
    max = std::log(max);

    val = val * (max - min) + min;
    val = std::exp(val);

    return val;
  };

  const auto log2norm = [](float min, float max, float val) -> float
  {
    constexpr float eps = std::numeric_limits<float>::epsilon();

    if (std::abs(val) < eps)
    {
      return 0;
    }

    min = std::max(min, 0.1f);

    min = std::log(min);
    max = std::log(max);

    val = std::log(val);
    val =  (val - min) / (max - min);

    return val;
  };

  add("bypass", new juce::AudioParameterBool(
    { "bypass", schema }, "Bypass", false,
    juce::AudioParameterBoolAttributes()));

  add("normalize", new juce::AudioParameterBool(
    { "normalize", schema }, "Normalize", false,
    juce::AudioParameterBoolAttributes()));

  add("frequency", new juce::AudioParameterFloat(
    { "frequency", schema }, "Frequency",
    juce::NormalisableRange<float>(20.0f, 20'000.0f, norm2log, log2norm), 1'000.0f,
    juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(fmt2int)
      .withLabel("hz")));

  add("quality", new juce::AudioParameterFloat(
    { "quality", schema }, "Quality",
    juce::NormalisableRange<float>(0.1f, 10.0f, norm2log, log2norm), 1.0f,
    juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(fmt2flt)
      .withLabel("q")));

  add("weights", new juce::AudioParameterFloat(
    { "dry", schema }, "Dry level",
    juce::NormalisableRange<float>(0.0f, 10.0f, norm2log, log2norm), 1.0f,
    juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(fmt2log)
      .withLabel("dB")));

  add("weights", new juce::AudioParameterFloat(
    { "wet.hp", schema }, "HP level",
    juce::NormalisableRange<float>(0.0f, 10.0f, norm2log, log2norm), 0.0f,
    juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(fmt2log)
      .withLabel("dB")));

  add("weights", new juce::AudioParameterFloat(
    { "wet.lp", schema }, "LP level",
    juce::NormalisableRange<float>(0.0f, 10.0f, norm2log, log2norm), 0.0f,
    juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(fmt2log)
      .withLabel("dB")));

  add("weights", new juce::AudioParameterFloat(
    { "wet.bp", schema }, "BP level",
    juce::NormalisableRange<float>(0.0f, 10.0f, norm2log, log2norm), 0.0f,
    juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(fmt2log)
      .withLabel("dB")));

  add("weights", new juce::AudioParameterFloat(
    { "wet.br", schema }, "BR level",
    juce::NormalisableRange<float>(0.0f, 10.0f, norm2log, log2norm), 0.0f,
    juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(fmt2log)
      .withLabel("dB")));

  add("gain", new juce::AudioParameterFloat(
    { "gain", schema }, "Input level",
    juce::NormalisableRange<float>(0.0f, 10.0f, norm2log, log2norm), 1.0f,
    juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(fmt2log)
      .withLabel("dB")));

  add("volume", new juce::AudioParameterFloat(
    { "volume", schema }, "Output level",
    juce::NormalisableRange<float>(0.0f, 10.0f, norm2log, log2norm), 1.0f,
    juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(fmt2log)
      .withLabel("dB")));
}

void Parameters::onbypass(std::function<void()> callback)
{
  call("bypass", callback);
}

void Parameters::onnormalize(std::function<void()> callback)
{
  call("normalize", callback);
}

void Parameters::onfrequency(std::function<void()> callback)
{
  call("frequency", callback);
}

void Parameters::onquality(std::function<void()> callback)
{
  call("quality", callback);
}

void Parameters::onweights(std::function<void()> callback)
{
  call("weights", callback);
}

void Parameters::ongain(std::function<void()> callback)
{
  call("gain", callback);
}

void Parameters::onvolume(std::function<void()> callback)
{
  call("volume", callback);
}


bool Parameters::bypass() const
{
  return get<bool>("bypass");
}

bool Parameters::normalize() const
{
  return get<bool>("normalize");
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

double Parameters::gain() const
{
  return get<double>("gain");
}

double Parameters::volume() const
{
  return get<double>("volume");
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
    read<bool>("normalize", *xml);
    read<double>("frequency", *xml);
    read<double>("quality", *xml);
    read<double>("dry", *xml);
    read<double>("wet.hp", *xml);
    read<double>("wet.lp", *xml);
    read<double>("wet.bp", *xml);
    read<double>("wet.br", *xml);
    read<double>("gain", *xml);
    read<double>("volume", *xml);
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
    write<bool>("normalize", *xml);
    write<double>("frequency", *xml);
    write<double>("quality", *xml);
    write<double>("dry", *xml);
    write<double>("wet.hp", *xml);
    write<double>("wet.lp", *xml);
    write<double>("wet.bp", *xml);
    write<double>("wet.br", *xml);
    write<double>("gain", *xml);
    write<double>("volume", *xml);

    LOG(xml->toString(juce::XmlElement::TextFormat().withoutHeader()));

    juce::AudioProcessor::copyXmlToBinary(*xml, data);
  }
  catch(const std::exception& exception)
  {
    juce::ignoreUnused(exception);

    LOG(exception.what());
  }
}
