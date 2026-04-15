#include "controls/DirektToggle.h"

#include "core/DirektParameterHistory.h"

namespace DirektDSP
{

DirektToggle::DirektToggle (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID,
                            const juce::String& labelText, DirektParameterHistory* history)
    : parameter (apvts.getParameter (paramID)), parameterHistory (history)
{
    button.setButtonText (labelText);
    addAndMakeVisible (button);

    if (parameter != nullptr)
    {
        lastValue = parameter->getValue();
    }

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, paramID, button);

    button.onClick = [this]
    {
        if (parameter == nullptr)
        {
            return;
        }

        auto const newValue = parameter->getValue();
        if (parameterHistory != nullptr)
        {
            parameterHistory->pushChange (parameter->getParameterID(), lastValue, newValue);
        }
        lastValue = newValue;
    };
}

void DirektToggle::resized()
{
    button.setBounds (getLocalBounds());
}

} // namespace DirektDSP
