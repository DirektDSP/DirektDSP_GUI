#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace DirektDSP
{

class DirektKnob : public juce::Component
{
public:
    DirektKnob (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID, const juce::String& labelText);

    void resized() override;

    juce::Slider& getSlider() { return slider; }

private:
    juce::Slider slider;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektKnob)
};

} // namespace DirektDSP
