#include <DigitalStateVariableFilterPlugin/Processor.h>

#include <DigitalStateVariableFilterPlugin/Chronometer.h>
#include <DigitalStateVariableFilterPlugin/Logger.h>

Processor::Processor() :
  juce::AudioProcessor(
    juce::AudioProcessor::BusesProperties()
      .withInput("Input",   juce::AudioChannelSet::stereo(), true)
      .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
  parameters = std::make_unique<Parameters>(*this);

  parameters->onfrequency([&]()
  {
    std::lock_guard lock(mutex);

    for (auto& effect : effects)
    {
      effect->frequency(parameters->frequency());
    }
  });

  parameters->onquality([&]()
  {
    std::lock_guard lock(mutex);

    for (auto& effect : effects)
    {
      effect->quality(parameters->quality());
    }
  });

  parameters->onnormalize([&]()
  {
    std::lock_guard lock(mutex);

    for (auto& effect : effects)
    {
      effect->normalize(parameters->normalize());
    }
  });

  parameters->ongain([&]()
  {
    std::lock_guard lock(mutex);

    for (auto& effect : effects)
    {
      effect->gain(parameters->gain());
    }
  });

  parameters->onvolume([&]()
  {
    std::lock_guard lock(mutex);

    for (auto& effect : effects)
    {
      effect->volume(parameters->volume());
    }
  });

  parameters->onweights([&]()
  {
    std::lock_guard lock(mutex);

    for (auto& effect : effects)
    {
      effect->weights(parameters->weights());
    }
  });
}

const juce::String Processor::getName() const
{
  return juce::String::formatted(
    "%s v%s",
    ProjectInfo::projectName,
    ProjectInfo::versionString);
}

bool Processor::hasEditor() const { return true; }
juce::AudioProcessorEditor* Processor::createEditor() { return new Editor(*this); }

bool Processor::isMidiEffect() const { return false; }
bool Processor::acceptsMidi() const { return false; }
bool Processor::producesMidi() const { return false; }
int  Processor::getNumPrograms() { return 1; }
int  Processor::getCurrentProgram() { return 0; }
void Processor::setCurrentProgram(int index) { juce::ignoreUnused(index); }
void Processor::changeProgramName(int index, const juce::String& name) { juce::ignoreUnused(index, name); }
const juce::String Processor::getProgramName(int index) { juce::ignoreUnused(index); return {}; }

double Processor::getTailLengthSeconds() const { return 0; }
juce::AudioProcessorParameter* Processor::getBypassParameter() const { return parameters->get("bypass"); }

bool Processor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  return true;
}

void Processor::getStateInformation(juce::MemoryBlock& data)
{
  LOG("Save plugin state");

  parameters->save(data);
}

void Processor::setStateInformation(const void* data, int size)
{
  LOG("Load plugin state");

  parameters->load(data, size);
}

void Processor::prepareToPlay(double samplerate, int blocksize)
{
  std::lock_guard lock(mutex);

  LOG("Release resources");

  config.reset();
  effects.clear();

  if (samplerate < 1)
  {
    LOG("Prepare to play (invalid samplerate %g)", samplerate);
    return;
  }

  if (blocksize < 1)
  {
    LOG("Prepare to play (invalid blocksize %d)", blocksize);
    return;
  }

  LOG("Prepare to play (samplerate %g, blocksize %d)", samplerate, blocksize);

  config =
  {
    .samplerate = samplerate,
    .blocksize = blocksize
  };

  try
  {
    for (auto i = 0; i < 2; ++i)
    {
      const auto frequency = parameters->frequency();
      const auto quality = parameters->quality();
      const auto normalize = parameters->normalize();
      const auto gain = parameters->gain();
      const auto volume = parameters->volume();
      const auto weights = parameters->weights();

      auto effect = std::make_unique<Effect>(samplerate);

      effect->frequency(frequency);
      effect->quality(quality);
      effect->normalize(normalize);
      effect->gain(gain);
      effect->volume(volume);
      effect->weights(weights);

      effects.push_back(std::move(effect));

      if (i < 1)
      {
        const auto latency = effects.at(0)->latency();

        LOG("Latency %d (%d ms)", latency, static_cast<int>(1e+3 * latency / samplerate));

        setLatencySamples(latency);
      }
    }
  }
  catch(const std::exception& exception)
  {
    juce::ignoreUnused(exception);

    LOG(exception.what());

    effects.clear();
  }
}

void Processor::releaseResources()
{
  std::lock_guard lock(mutex);

  LOG("Release resources");

  config.reset();
  effects.clear();
}

void Processor::processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi)
{
  juce::ignoreUnused(midi);

  juce::ScopedNoDenormals dontcare;

  std::lock_guard lock(mutex);

  const int input_channels  = getTotalNumInputChannels();
  const int output_channels = getTotalNumOutputChannels();
  const int channel_samples = audio.getNumSamples();

  if (input_channels < 1)
  {
    LOG("Skip block (invalid number of input channels %d)", input_channels);
    return;
  }

  if (output_channels < 1)
  {
    LOG("Skip block (invalid number of output channels %d)", output_channels);
    return;
  }

  if (channel_samples < 1)
  {
    LOG("Skip block (invalid number of samples per block %d)", channel_samples);
    return;
  }

  const auto process_stereo_input = [&]()
  {
    const int processible_channels = std::min(
    {
      input_channels,
      output_channels,
      static_cast<int>(effects.size())
    });

    const int remaining_channels = std::min(
    {
      output_channels,
      static_cast<int>(effects.size())
    });

    for (int channel = 0; channel < processible_channels; ++channel)
    {
      auto input = std::span<const float>(
        audio.getReadPointer(channel),
        static_cast<size_t>(channel_samples));

      auto output = std::span<float>(
        audio.getWritePointer(channel),
        static_cast<size_t>(channel_samples));

      if (parameters->bypass())
      {
        effects.at(channel)->dry(input, output);
      }
      else
      {
        effects.at(channel)->wet(input, output);
      }
    }

    for (int channel = processible_channels; channel < remaining_channels; ++channel)
    {
      audio.copyFrom(channel, 0, audio, 0, 0, channel_samples);
    }
  };

  const auto process_stereo_output = [&](const std::string& error = "")
  {
    if (!error.empty())
    {
      LOG("Copy input to output (%s)", error.c_str());
    }

    for (int channel = 1; channel < output_channels; ++channel)
    {
      audio.copyFrom(channel, 0, audio, 0, 0, channel_samples);
    }
  };

  TIC();

  if (!config)
  {
    process_stereo_output("plugin is not prepared");
  }
  else if (!effects.size())
  {
    process_stereo_output("effects are not initialized");
  }
  else
  {
    try
    {
      process_stereo_input();
    }
    catch(const std::exception& exception)
    {
      process_stereo_output(exception.what());
    }
  }

  TOC();

  if (LAP())
  {
    const auto [sr, bs] = config.value_or(Config{});

    juce::ignoreUnused(sr, bs);

    LOG(CHRONOMETRY(sr, bs));
  }
}
