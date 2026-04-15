#include "controls/DirektMacroControl.h"

#include <cmath>

#include "theme/DirektColours.h"

namespace DirektDSP
{

namespace
{
float mapMacroValue (const MacroTarget& target, float normalizedMacroValue)
{
    auto const clampedMacro = juce::jlimit (0.0f, 1.0f, normalizedMacroValue);
    auto const clampedExponent = juce::jmax (0.01f, target.curveExponent);
    auto const curved = std::pow (clampedMacro, clampedExponent);
    auto const minNorm = juce::jlimit (0.0f, 1.0f, target.minNormalized);
    auto const maxNorm = juce::jlimit (0.0f, 1.0f, target.maxNormalized);
    return juce::jmap (curved, minNorm, maxNorm);
}
} // namespace

DirektMacroControl::DirektMacroControl (const juce::String& labelText, std::vector<MacroTarget> targetsToOwn)
    : targets (std::move (targetsToOwn))
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setRange (0.0, 1.0, 0.0001);
    slider.setValue (0.0, juce::dontSendNotification);
    slider.setTextValueSuffix (" macro");
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 16);
    slider.setTextBoxIsEditable (true);
    slider.setColour (juce::Slider::textBoxTextColourId, Colours::textBright);
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    slider.onValueChange = [this] { applyMacroValue (static_cast<float> (slider.getValue())); };
    addAndMakeVisible (slider);

    label.setText (labelText, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, Colours::textDim);
    label.setFont (juce::Font (juce::FontOptions (11.0F)));
    addAndMakeVisible (label);
}

void DirektMacroControl::resized()
{
    auto bounds = getLocalBounds();
    auto const labelHeight = 16;
    label.setBounds (bounds.removeFromBottom (labelHeight));
    slider.setBounds (bounds);
}

void DirektMacroControl::applyMacroValue (float normalizedValue)
{
    for (const auto& target : targets)
    {
        if (target.parameter == nullptr)
        {
            continue;
        }

        auto const mappedValue = mapMacroValue (target, normalizedValue);
        target.parameter->beginChangeGesture();
        target.parameter->setValueNotifyingHost (mappedValue);
        target.parameter->endChangeGesture();
    }
}

} // namespace DirektDSP
