#include "controls/DirektKnob.h"

#include "core/DirektParameterHistory.h"
#include "theme/DirektColours.h"

namespace DirektDSP
{

DirektKnob::DirektKnob (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID,
                        const juce::String& labelText, DirektParameterHistory* history)
    : parameter (apvts.getParameter (paramID)), parameterHistory (history)
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
    label.setFont (juce::Font (juce::FontOptions (11.0F)));
    addAndMakeVisible (label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, paramID, slider);

    slider.onDragStart = [this]
    {
        if (parameter != nullptr)
        {
            dragStartValue = parameter->getValue();
        }
    };

    slider.onDragEnd = [this]
    {
        if (parameter == nullptr || !dragStartValue.has_value())
        {
            return;
        }

        commitChange (*dragStartValue, parameter->getValue());
        dragStartValue.reset();
    };
}

void DirektKnob::resized()
{
    auto bounds = getLocalBounds();
    auto labelHeight = 16;
    label.setBounds (bounds.removeFromBottom (labelHeight));
    slider.setBounds (bounds);
}

void DirektKnob::commitChange (float oldValue, float newValue) const
{
    if (parameterHistory != nullptr)
    {
        parameterHistory->pushChange (parameter->getParameterID(), oldValue, newValue);
    }
}

} // namespace DirektDSP
