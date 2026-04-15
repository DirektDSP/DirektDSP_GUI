#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace DirektDSP
{

class DirektParameterHistory;

class DirektToggle : public juce::Component
{
public:
    /**
     * @brief Creates toggle parameter control with optional undo tracking.
     *
     * @param apvts Processor parameter state.
     * @param paramID Target parameter identifier.
     * @param labelText Button label.
     * @param parameterHistory Optional edit history sink for undo/redo.
     */
    DirektToggle (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID, const juce::String& labelText,
                  DirektParameterHistory* parameterHistory = nullptr);

    void resized() override;

    juce::ToggleButton& getButton() { return button; }

private:
    juce::ToggleButton button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;
    juce::RangedAudioParameter* parameter = nullptr;
    DirektParameterHistory* parameterHistory = nullptr;
    float lastValue = 0.0F;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektToggle)
};

} // namespace DirektDSP
