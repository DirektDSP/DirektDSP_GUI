#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <optional>

namespace DirektDSP
{

class DirektParameterHistory;

class DirektKnob : public juce::Component
{
public:
    /**
     * @brief Creates rotary parameter control with optional undo tracking.
     *
     * @param apvts Processor parameter state.
     * @param paramID Target parameter identifier.
     * @param labelText Caption shown below knob.
     * @param parameterHistory Optional edit history sink for undo/redo.
     */
    DirektKnob (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID, const juce::String& labelText,
                DirektParameterHistory* parameterHistory = nullptr);

    void resized() override;

    juce::Slider& getSlider() { return slider; }

private:
    void commitChange (float oldValue, float newValue) const;

    juce::Slider slider;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    std::unique_ptr<juce::MouseListener> midiLearnMouseListener;
    juce::RangedAudioParameter* parameter = nullptr;
    DirektParameterHistory* parameterHistory = nullptr;
    std::optional<float> dragStartValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektKnob)
};

} // namespace DirektDSP
