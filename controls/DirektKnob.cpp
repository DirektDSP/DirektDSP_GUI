#include "controls/DirektKnob.h"
#include "theme/DirektColours.h"

namespace DirektDSP
{

DirektKnob::DirektKnob (juce::AudioProcessorValueTreeState& apvts,
                         const juce::String& paramID,
                         const juce::String& labelText)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 16);
    slider.setTextBoxIsEditable (true);
    slider.setColour (juce::Slider::textBoxTextColourId, Colours::textBright);
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    addAndMakeVisible (slider);

    label.setText (labelText, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, Colours::textDim);
    label.setFont (juce::Font (juce::FontOptions (11.0f)));
    addAndMakeVisible (label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, paramID, slider);
}

void DirektKnob::resized()
{
    auto bounds = getLocalBounds();
    auto labelHeight = 16;
    label.setBounds (bounds.removeFromBottom (labelHeight));
    slider.setBounds (bounds);
}

} // namespace DirektDSP
