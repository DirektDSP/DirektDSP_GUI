#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

#include <atomic>
#include <map>

namespace DirektDSP
{

class DirektLookAndFeel;
class DirektParameterHistory;

struct BuildContext
{
    juce::AudioProcessorValueTreeState& apvts;
    DirektLookAndFeel& lookAndFeel;
    DirektParameterHistory* parameterHistory = nullptr;
    std::map<juce::String, const std::atomic<float>*> meterSources;
};

} // namespace DirektDSP
