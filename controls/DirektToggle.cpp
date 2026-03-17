#include "controls/DirektToggle.h"

namespace DirektDSP
{

DirektToggle::DirektToggle (juce::AudioProcessorValueTreeState& apvts,
                             const juce::String& paramID,
                             const juce::String& labelText)
{
    button.setButtonText (labelText);
    addAndMakeVisible (button);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, paramID, button);
}

void DirektToggle::resized()
{
    button.setBounds (getLocalBounds());
}

} // namespace DirektDSP
