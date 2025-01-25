#include <DigitalStateVariableFilterPlugin/Parameters.h>

Parameters::Parameters(juce::AudioProcessor& process) :
  XmlParameters(process, 1)
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

    if (std::abs(val) < eps)
    {
      return juce::String::formatted("%s (%.2f)", "-inf", val); // -inf
    }
    else
    {
      const float log = std::round(20 * std::log10(val));

      if (log != 0.f)
      {
        return juce::String::formatted("%+.0f (%.2f)", log, val); // signed
      }
      else
      {
        return juce::String::formatted("%.0f (%.2f)", 0.f, val); // unsigned
      }
    }
  };

  const auto fmt2phi = [](int val, int)
  {
    if (val > 0)
    {
      return juce::String::formatted("on (%+i)", val); // signed
    }
    else if (val < 0)
    {
      return juce::String::formatted("inv (%+i)", val); // signed
    }
    else
    {
      return juce::String::formatted("off (%i)", val); // unsigned
    }
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

  add("frequency", new juce::AudioParameterFloat(
    { "frequency", schema }, "Frequency",
    juce::NormalisableRange<float>(20.0f, 20'000.0f, norm2log, log2norm), 1'000.0f,
    juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(fmt2int)
      .withLabel("Hz")));

  add("quality", new juce::AudioParameterFloat(
    { "quality", schema }, "Quality",
    juce::NormalisableRange<float>(0.1f, 10.0f, norm2log, log2norm), 1.0f,
    juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(fmt2flt)
      .withLabel("Q")));

  add("weights", new juce::AudioParameterFloat(
    { "dry", schema }, "Dry level",
    juce::NormalisableRange<float>(0.0f, 10.0f, norm2log, log2norm), 1.0f,
    juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(fmt2log)
      .withLabel("dB")));

  add("weights", new juce::AudioParameterInt(
    { "dry.left", schema }, "",
    -1, +1, 1,
    juce::AudioParameterIntAttributes()
      .withStringFromValueFunction(fmt2phi)
      .withLabel("L")));

  add("weights", new juce::AudioParameterInt(
    { "dry.right", schema }, "",
    -1, +1, 1,
    juce::AudioParameterIntAttributes()
      .withStringFromValueFunction(fmt2phi)
      .withLabel("R")));

  const std::vector<std::pair<std::string, std::string>> wet =
  {
    { "hp", "HP" },
    { "lp", "LP" },
    { "bp", "BP" },
    { "br", "BR" },
  };

  for (auto& [id, name] : wet)
  {
    add("weights", new juce::AudioParameterFloat(
      { "wet." + id, schema }, name + " level",
      juce::NormalisableRange<float>(0.0f, 10.0f, norm2log, log2norm), 0.0f,
      juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction(fmt2log)
        .withLabel("dB")));

    add("weights", new juce::AudioParameterInt(
      { "wet." + id + ".left", schema }, "",
      -1, +1, 1,
      juce::AudioParameterIntAttributes()
        .withStringFromValueFunction(fmt2phi)
        .withLabel("L")));

    add("weights", new juce::AudioParameterInt(
      { "wet." + id + ".right", schema }, "",
      -1, +1, 1,
      juce::AudioParameterIntAttributes()
        .withStringFromValueFunction(fmt2phi)
        .withLabel("R")));
  }
}

void Parameters::onbypass(std::function<void()> callback)
{
  call("bypass", std::move(callback));
}

void Parameters::onnormalize(std::function<void()> callback)
{
  call("normalize", std::move(callback));
}

void Parameters::ongain(std::function<void()> callback)
{
  call("gain", std::move(callback));
}

void Parameters::onvolume(std::function<void()> callback)
{
  call("volume", std::move(callback));
}

void Parameters::onfrequency(std::function<void()> callback)
{
  call("frequency", std::move(callback));
}

void Parameters::onquality(std::function<void()> callback)
{
  call("quality", std::move(callback));
}

void Parameters::onweights(std::function<void()> callback)
{
  call("weights", std::move(callback));
}

bool Parameters::bypass() const
{
  return get<bool>("bypass");
}

bool Parameters::normalize() const
{
  return get<bool>("normalize");
}

double Parameters::gain() const
{
  return get<double>("gain");
}

double Parameters::volume() const
{
  return get<double>("volume");
}

double Parameters::frequency() const
{
  return get<double>("frequency");
}

double Parameters::quality() const
{
  return get<double>("quality");
}

std::vector<double> Parameters::weights(const int channel) const
{
  if (channel < 1)
  {
    return
    {
      get<double>("dry")    * get<double>("dry.left"),
      get<double>("wet.hp") * get<double>("wet.hp.left"),
      get<double>("wet.bp") * get<double>("wet.bp.left"),
      get<double>("wet.lp") * get<double>("wet.lp.left"),
      get<double>("wet.br") * get<double>("wet.br.left")
    };
  }
  else
  {
    return
    {
      get<double>("dry")    * get<double>("dry.right"),
      get<double>("wet.hp") * get<double>("wet.hp.right"),
      get<double>("wet.bp") * get<double>("wet.bp.right"),
      get<double>("wet.lp") * get<double>("wet.lp.right"),
      get<double>("wet.br") * get<double>("wet.br.right")
    };
  }
}
