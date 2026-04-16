#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace DirektDSP
{

class DirektParameterHistory;

class DirektComboBox : public juce::Component
{
public:
    /**
     * @brief Creates choice parameter control with optional undo tracking.
     *
     * @param apvts Processor parameter state.
     * @param paramID Target parameter identifier.
     * @param labelText Caption shown below control.
     * @param parameterHistory Optional edit history sink for undo/redo.
     */
    DirektComboBox (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID,
                    const juce::String& labelText, DirektParameterHistory* parameterHistory = nullptr);

    void resized() override;

    juce::ComboBox& getComboBox() { return combo; }

private:
    juce::ComboBox combo;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;
    std::unique_ptr<juce::MouseListener> midiLearnMouseListener;
    juce::RangedAudioParameter* parameter = nullptr;
    DirektParameterHistory* parameterHistory = nullptr;
    float lastValue = 0.0F;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektComboBox)
};

} // namespace DirektDSP
