#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace DirektDSP
{

class DirektToggle : public juce::Component
{
public:
    DirektToggle (juce::AudioProcessorValueTreeState& apvts,
                  const juce::String& paramID,
                  const juce::String& labelText);

    void resized() override;

    juce::ToggleButton& getButton() { return button; }

private:
    juce::ToggleButton button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektToggle)
};

} // namespace DirektDSP
