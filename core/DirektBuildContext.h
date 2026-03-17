#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include <map>

namespace DirektDSP
{

class DirektLookAndFeel;

struct BuildContext
{
    juce::AudioProcessorValueTreeState& apvts;
    DirektLookAndFeel&                  lookAndFeel;
    std::map<juce::String, const std::atomic<float>*> meterSources;
};

} // namespace DirektDSP
