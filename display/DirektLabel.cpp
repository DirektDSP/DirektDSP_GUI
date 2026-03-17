#include "display/DirektLabel.h"

namespace DirektDSP
{

DirektLabel::DirektLabel (const juce::String& text, Style style)
{
    setText (text, juce::dontSendNotification);
    setJustificationType (juce::Justification::centredLeft);

    switch (style)
    {
        case Title:
            setFont (juce::Font (juce::FontOptions (18.0f).withStyle ("Bold")));
            setColour (textColourId, Colours::textBright);
            break;
        case Section:
            setFont (juce::Font (juce::FontOptions (13.0f).withStyle ("Bold")));
            setColour (textColourId, Colours::textLabel);
            break;
        case Body:
        default:
            setFont (juce::Font (juce::FontOptions (12.0f)));
            setColour (textColourId, Colours::textDim);
            break;
    }
}

} // namespace DirektDSP
