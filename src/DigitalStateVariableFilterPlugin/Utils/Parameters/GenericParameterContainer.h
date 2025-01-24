#pragma once

#include <JuceHeader.h>

#include <DigitalStateVariableFilterPlugin/Utils/Parameters/GenericParameterListener.h>
#include <DigitalStateVariableFilterPlugin/Utils/Parameters/GenericParameterSubscription.h>

class GenericParameterContainer
{

public:

  GenericParameterContainer(juce::AudioProcessor& process) :
    process(process)
  {
  }

  juce::RangedAudioParameter* get(const std::string& id) const
  {
    return parameters.at(id);
  }

  template<typename T>
  T get(const std::string& id) const
  {
    static_assert(missing_template_specialization<T>::value);
  }

  template<typename T>
  void set(const std::string& id, const T value) const
  {
    static_assert(missing_template_specialization<T>::value);
  }

  std::shared_ptr<GenericParameterSubscription> subscribe(const std::string& id, const std::function<void()> callback) const
  {
    return std::make_shared<GenericParameterSubscription>(get(id), callback);
  }

protected:

  void add(const std::string& ns, juce::RangedAudioParameter* parameter)
  {
    auto id = parameter->getParameterID().toStdString();

    auto callback = [ns, this]()
    {
      if (callbacks.contains(ns))
      {
        for (const auto& callback : callbacks.at(ns))
        {
          callback();
        }
      }
    };

    auto subscription = std::make_unique<GenericParameterSubscription>(parameter, callback);

    parameters[id] = parameter;
    subscriptions.push_back(std::move(subscription));
    process.addParameter(parameter);
  }

  void call(const std::string& ns, std::function<void()> callback)
  {
    callbacks[ns].push_back(std::move(callback));
  }

  void visit(std::function<void(const std::string& id, const juce::RangedAudioParameter* parameter)> callback)
  {
    for (const auto& [id, parameter] : parameters)
    {
      callback(id, parameter);
    }
  }

private:

  template<typename T>
  struct missing_template_specialization : std::false_type {};

  juce::AudioProcessor& process;

  std::map<std::string, juce::RangedAudioParameter*> parameters;
  std::vector<std::unique_ptr<GenericParameterSubscription>> subscriptions;
  std::map<std::string, std::vector<std::function<void()>>> callbacks;

};

template<>
inline bool GenericParameterContainer::get<bool>(const std::string& id) const
{
  auto* parameter = dynamic_cast<juce::AudioParameterBool*>(parameters.at(id));

  return *parameter;
}

template<>
inline int GenericParameterContainer::get<int>(const std::string& id) const
{
  auto* parameter = dynamic_cast<juce::AudioParameterInt*>(parameters.at(id));

  return *parameter;
}

template<>
inline float GenericParameterContainer::get<float>(const std::string& id) const
{
  auto* parameter = dynamic_cast<juce::AudioParameterFloat*>(parameters.at(id));

  return *parameter;
}

template<>
inline double GenericParameterContainer::get<double>(const std::string& id) const
{
  auto* parameter = dynamic_cast<juce::AudioParameterFloat*>(parameters.at(id));

  return static_cast<double>(*parameter);
}

template<>
inline std::string GenericParameterContainer::get<std::string>(const std::string& id) const
{
  auto* parameter = dynamic_cast<juce::AudioParameterChoice*>(parameters.at(id));

  return parameter->getCurrentChoiceName().toStdString();
}

template<>
inline void GenericParameterContainer::set<bool>(const std::string& id, const bool value) const
{
  auto* parameter = dynamic_cast<juce::AudioParameterBool*>(parameters.at(id));

  *parameter = value;
}

template<>
inline void GenericParameterContainer::set<int>(const std::string& id, const int value) const
{
  auto* parameter = dynamic_cast<juce::AudioParameterInt*>(parameters.at(id));

  *parameter = value;
}

template<>
inline void GenericParameterContainer::set<float>(const std::string& id, const float value) const
{
  auto* parameter = dynamic_cast<juce::AudioParameterFloat*>(parameters.at(id));

  *parameter = value;
}

template<>
inline void GenericParameterContainer::set<double>(const std::string& id, const double value) const
{
  auto* parameter = dynamic_cast<juce::AudioParameterFloat*>(parameters.at(id));

  *parameter = static_cast<float>(value);
}

template<>
inline void GenericParameterContainer::set<std::string>(const std::string& id, const std::string value) const
{
  auto* parameter = dynamic_cast<juce::AudioParameterChoice*>(parameters.at(id));

  const juce::StringArray& choices = parameter->choices;
  const int index = choices.indexOf(std::move(value), true);

  if (index >= 0)
  {
    *parameter = index;
  }
}
