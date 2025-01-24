#pragma once

#include <JuceHeader.h>

#include <DigitalStateVariableFilterPlugin/Utils/Logger.h>
#include <DigitalStateVariableFilterPlugin/Utils/Parameters/GenericParameterContainer.h>

class XmlParameters : public GenericParameterContainer
{

public:

  XmlParameters(juce::AudioProcessor& process, const int schema, const std::string tag = ProjectInfo::projectName) :
    GenericParameterContainer(process),
    schema(schema),
    tag(std::move(tag))
  {
  }

  void load(const void* data, const int size);
  void save(juce::MemoryBlock& data);

protected:

  const int schema;
  const std::string tag;

private:

  template<typename T>
  struct missing_template_specialization : std::false_type {};

  template<typename T>
  void read(const std::string& id, juce::XmlElement& parent) const
  {
    static_assert(missing_template_specialization<T>::value);
  }

  template<typename T>
  void write(const std::string& id, juce::XmlElement& parent) const
  {
    static_assert(missing_template_specialization<T>::value);
  }

};

template<>
inline void XmlParameters::read<bool>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.getChildByName(id);
  if (!child) { return; }

  juce::String value = child->getAllSubText();
  if (value.isEmpty()) { return; }

  set<bool>(id, value == "true");
}

template<>
inline void XmlParameters::read<int>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.getChildByName(id);
  if (!child) { return; }

  juce::String value = child->getAllSubText();
  if (value.isEmpty()) { return; }

  set<int>(id, value.getIntValue());
}

template<>
inline void XmlParameters::read<float>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.getChildByName(id);
  if (!child) { return; }

  juce::String value = child->getAllSubText();
  if (value.isEmpty()) { return; }

  set<float>(id, value.getFloatValue());
}

template<>
inline void XmlParameters::read<std::string>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.getChildByName(id);
  if (!child) { return; }

  juce::String value = child->getAllSubText();
  if (value.isEmpty()) { return; }

  set<std::string>(id, value.toStdString());
}

template<>
inline void XmlParameters::write<bool>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.createNewChildElement(id);
  child->addTextElement(get<bool>(id) ? "true" : "false");
}

template<>
inline void XmlParameters::write<int>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.createNewChildElement(id);
  child->addTextElement(juce::String(get<int>(id)));
}

template<>
inline void XmlParameters::write<float>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.createNewChildElement(id);
  child->addTextElement(juce::String(get<float>(id)));
}

template<>
inline void XmlParameters::write<std::string>(const std::string& id, juce::XmlElement& parent) const
{
  juce::XmlElement* child = parent.createNewChildElement(id);
  child->addTextElement(juce::String(get<std::string>(id)));
}

inline void XmlParameters::load(const void* data, const int size)
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

    if (xml->hasTagName(tag) != true) { return; }
    if (xml->getIntAttribute("schema") != schema) { return; }

    visit([&](const std::string& id, const juce::RangedAudioParameter* parameter)
    {
      if (dynamic_cast<const juce::AudioParameterBool*>(parameter))
      {
        read<bool>(id, *xml);
        return;
      }

      if (dynamic_cast<const juce::AudioParameterInt*>(parameter))
      {
        read<int>(id, *xml);
        return;
      }

      if (dynamic_cast<const juce::AudioParameterFloat*>(parameter))
      {
        read<float>(id, *xml);
        return;
      }

      if (dynamic_cast<const juce::AudioParameterChoice*>(parameter))
      {
        read<std::string>(id, *xml);
        return;
      }

      throw std::invalid_argument("Unsupported parameter type!");
    });
  }
  catch(const std::exception& exception)
  {
    juce::ignoreUnused(exception);

    LOG(exception.what());
  }
}

inline void XmlParameters::save(juce::MemoryBlock& data)
{
  try
  {
    auto xml = std::make_unique<juce::XmlElement>(juce::String(tag));

    xml->setAttribute("schema", schema);

    visit([&](const std::string& id, const juce::RangedAudioParameter* parameter)
    {
      if (dynamic_cast<const juce::AudioParameterBool*>(parameter))
      {
        write<bool>(id, *xml);
        return;
      }

      if (dynamic_cast<const juce::AudioParameterInt*>(parameter))
      {
        write<int>(id, *xml);
        return;
      }

      if (dynamic_cast<const juce::AudioParameterFloat*>(parameter))
      {
        write<float>(id, *xml);
        return;
      }

      if (dynamic_cast<const juce::AudioParameterChoice*>(parameter))
      {
        write<std::string>(id, *xml);
        return;
      }

      throw std::invalid_argument("Unsupported parameter type!");
    });

    LOG(xml->toString(juce::XmlElement::TextFormat().withoutHeader()));

    juce::AudioProcessor::copyXmlToBinary(*xml, data);
  }
  catch(const std::exception& exception)
  {
    juce::ignoreUnused(exception);

    LOG(exception.what());
  }
}
