#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace DirektDSP
{

class DirektComboBox : public juce::Component
{
public:
    DirektComboBox (juce::AudioProcessorValueTreeState& apvts,
                    const juce::String& paramID,
                    const juce::String& labelText);

    void resized() override;

    juce::ComboBox& getComboBox() { return combo; }

private:
    juce::ComboBox combo;
    juce::Label    label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektComboBox)
};

} // namespace DirektDSP
