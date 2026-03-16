#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "DirektColours.h"

namespace Service { class PresetManager; }

namespace DirektDSP
{

class DirektHeader : public juce::Component
{
public:
    DirektHeader (const juce::String& pluginName,
                  juce::Colour accentColour,
                  Service::PresetManager& presetManager,
                  juce::AudioProcessorValueTreeState& apvts);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::String pluginName;
    juce::Colour accent;

    juce::TextButton prevBtn { "<" };
    juce::TextButton nextBtn { ">" };
    juce::Label      presetLabel;

    Service::PresetManager& presetManager;

    void updatePresetName();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektHeader)
};

} // namespace DirektDSP
