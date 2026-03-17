#include "controls/DirektComboBox.h"
#include "theme/DirektColours.h"

namespace DirektDSP
{

DirektComboBox::DirektComboBox (juce::AudioProcessorValueTreeState& apvts,
                                 const juce::String& paramID,
                                 const juce::String& labelText)
{
    // Auto-populate choices from AudioParameterChoice
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (paramID)))
    {
        auto choices = param->choices;
        for (int i = 0; i < choices.size(); ++i)
            combo.addItem (choices[i], i + 1);
    }

    addAndMakeVisible (combo);

    label.setText (labelText, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, Colours::textDim);
    label.setFont (juce::Font (juce::FontOptions (11.0f)));
    addAndMakeVisible (label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (apvts, paramID, combo);
}

void DirektComboBox::resized()
{
    auto bounds = getLocalBounds();
    auto labelHeight = 16;
    label.setBounds (bounds.removeFromBottom (labelHeight));
    combo.setBounds (bounds);
}

} // namespace DirektDSP
