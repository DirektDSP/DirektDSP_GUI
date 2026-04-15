#include "chrome/DirektFooter.h"

namespace DirektDSP
{

DirektFooter::DirektFooter()
{
    captureAButton.setTooltip ("Capture snapshot A");
    captureAButton.onClick = [this]
    {
        if (onCaptureSnapshotA)
        {
            onCaptureSnapshotA();
        }
    };
    addAndMakeVisible (captureAButton);

    captureBButton.setTooltip ("Capture snapshot B");
    captureBButton.onClick = [this]
    {
        if (onCaptureSnapshotB)
        {
            onCaptureSnapshotB();
        }
    };
    addAndMakeVisible (captureBButton);

    morphLabel.setText ("Morph", juce::dontSendNotification);
    morphLabel.setJustificationType (juce::Justification::centredRight);
    morphLabel.setColour (juce::Label::textColourId, Colours::textDim);
    morphLabel.setFont (juce::Font (juce::FontOptions (10.0F)));
    addAndMakeVisible (morphLabel);

    morphSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    morphSlider.setRange (0.0, 1.0, 0.001);
    morphSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    morphSlider.setDoubleClickReturnValue (true, 0.0);
    morphSlider.setTooltip ("Interpolate between snapshot A and B");
    morphSlider.onValueChange = [this]
    {
        if (onMorphChanged)
        {
            onMorphChanged (static_cast<float> (morphSlider.getValue()));
        }
    };
    addAndMakeVisible (morphSlider);

    setSnapshotAvailability (false, false);
}

void DirektFooter::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (Colours::bgFooter);
    g.fillRect (bounds);

    // Top divider
    g.setColour (Colours::divider);
    g.fillRect (getLocalBounds().removeFromTop (1));

    g.setColour (Colours::textDim);
    g.setFont (juce::Font (juce::FontOptions (10.0F)));

#if defined(VERSION)
    juce::String versionStr = "v" + juce::String (VERSION);
#else
    juce::String const versionStr = "";
#endif

    auto versionArea = getLocalBounds().removeFromRight (120);
    g.drawText ("DirektDSP " + versionStr, versionArea, juce::Justification::centredRight);
}

void DirektFooter::resized()
{
    auto bounds = getLocalBounds().reduced (6, 2);
    auto controlsArea = bounds.removeFromLeft (juce::jmax (180, getWidth() - 136));

    captureAButton.setBounds (controlsArea.removeFromLeft (28));
    controlsArea.removeFromLeft (4);
    captureBButton.setBounds (controlsArea.removeFromLeft (28));
    controlsArea.removeFromLeft (8);

    morphLabel.setBounds (controlsArea.removeFromLeft (40));
    morphSlider.setBounds (controlsArea);
}

void DirektFooter::setSnapshotAvailability (bool hasSnapshotA, bool hasSnapshotB)
{
    captureAButton.setColour (juce::TextButton::buttonColourId, hasSnapshotA ? Colours::accentBlue : Colours::bgPanel);
    captureAButton.setColour (juce::TextButton::textColourOffId, hasSnapshotA ? Colours::bgDark : Colours::textDim);
    captureBButton.setColour (juce::TextButton::buttonColourId, hasSnapshotB ? Colours::accentBlue : Colours::bgPanel);
    captureBButton.setColour (juce::TextButton::textColourOffId, hasSnapshotB ? Colours::bgDark : Colours::textDim);

    morphSlider.setEnabled (hasSnapshotA && hasSnapshotB);
}

} // namespace DirektDSP
