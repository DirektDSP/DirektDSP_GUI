#include "chrome/DirektHeader.h"

#include "Service/PresetManager.h"

namespace DirektDSP
{

DirektHeader::DirektHeader (const juce::String& name, juce::Colour accentCol, Service::PresetManager& pm,
                            juce::AudioProcessorValueTreeState& /*apvts*/)
    : pluginName (name), accent (accentCol), presetManager (pm)
{
    prevBtn.setColour (juce::TextButton::buttonColourId, Colours::bgHeader);
    prevBtn.setColour (juce::TextButton::textColourOffId, Colours::textDim);
    prevBtn.onClick = [this]
    {
        presetManager.loadPreviousPreset();
        updatePresetName();
    };
    addAndMakeVisible (prevBtn);

    nextBtn.setColour (juce::TextButton::buttonColourId, Colours::bgHeader);
    nextBtn.setColour (juce::TextButton::textColourOffId, Colours::textDim);
    nextBtn.onClick = [this]
    {
        presetManager.loadNextPreset();
        updatePresetName();
    };
    addAndMakeVisible (nextBtn);

    presetLabel.setJustificationType (juce::Justification::centred);
    presetLabel.setColour (juce::Label::textColourId, Colours::textBright);
    presetLabel.setFont (juce::Font (juce::FontOptions (13.0f)));
    presetLabel.setInterceptsMouseClicks (true, false);
    presetLabel.addMouseListener (this, false);
    addAndMakeVisible (presetLabel);

    updatePresetName();
}

void DirektHeader::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (Colours::bgHeader);
    g.fillRect (bounds);

    // Brand name
    g.setColour (Colours::textDim);
    g.setFont (juce::Font (juce::FontOptions (11.0f).withStyle ("Bold")));
    g.drawText ("DirektDSP", bounds.removeFromLeft (80.0f).toNearestInt(), juce::Justification::centred);

    // Plugin name
    g.setColour (accent);
    g.setFont (juce::Font (juce::FontOptions (15.0f).withStyle ("Bold")));
    auto nameArea = getLocalBounds().withWidth (160).withX (80);
    g.drawText (pluginName.toUpperCase(), nameArea, juce::Justification::centredLeft);

    // Bottom divider
    g.setColour (Colours::divider);
    g.fillRect (getLocalBounds().removeFromBottom (1));
}

void DirektHeader::resized()
{
    auto bounds = getLocalBounds();
    auto rightArea = bounds.removeFromRight (bounds.getWidth() / 2);

    auto presetArea = rightArea.reduced (4, 4);
    prevBtn.setBounds (presetArea.removeFromLeft (28));
    nextBtn.setBounds (presetArea.removeFromRight (28));
    presetLabel.setBounds (presetArea);
}

void DirektHeader::mouseDown (const juce::MouseEvent& e)
{
    if (e.eventComponent == &presetLabel && onPresetLabelClicked)
        onPresetLabelClicked();
}

void DirektHeader::updatePresetName()
{
    auto name = presetManager.getCurrentPreset();
    presetLabel.setText (name.isEmpty() ? "Init" : name, juce::dontSendNotification);
}

} // namespace DirektDSP
