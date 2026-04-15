#include "chrome/DirektHeader.h"
#include "Service/PresetManager.h"

namespace DirektDSP
{

DirektHeader::DirektHeader (const juce::String& name,
                             juce::Colour accentCol,
                             Service::PresetManager& pm,
                             juce::AudioProcessorValueTreeState& apvtsRef)
    : pluginName (name), accent (accentCol), apvts (apvtsRef), presetManager (pm)
{
    snapshotA = apvts.copyState();
    snapshotB = apvts.copyState();

    auto wireChromeButton = [] (juce::TextButton& b)
    {
        b.setColour (juce::TextButton::buttonColourId, Colours::bgHeader);
        b.setColour (juce::TextButton::textColourOffId, Colours::textDim);
    };

    abBtnA.setTooltip ("Compare A: recall slot A (current edits saved to other slot).");
    abBtnB.setTooltip ("Compare B: recall slot B (current edits saved to other slot).");
    wireChromeButton (abBtnA);
    wireChromeButton (abBtnB);
    abBtnA.onClick = [this] { switchToSlot (ABSlot::A); };
    abBtnB.onClick = [this] { switchToSlot (ABSlot::B); };
    addAndMakeVisible (abBtnA);
    addAndMakeVisible (abBtnB);
    styleABButtons();

    prevBtn.setColour (juce::TextButton::buttonColourId, Colours::bgHeader);
    prevBtn.setColour (juce::TextButton::textColourOffId, Colours::textDim);
    prevBtn.onClick = [this] {
        presetManager.loadPreviousPreset();
        updatePresetName();
        syncABSnapshotsAfterPresetLoad();
    };
    addAndMakeVisible (prevBtn);

    nextBtn.setColour (juce::TextButton::buttonColourId, Colours::bgHeader);
    nextBtn.setColour (juce::TextButton::textColourOffId, Colours::textDim);
    nextBtn.onClick = [this] {
        presetManager.loadNextPreset();
        updatePresetName();
        syncABSnapshotsAfterPresetLoad();
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
    g.drawText ("DirektDSP", bounds.removeFromLeft (80.0f).toNearestInt(),
                juce::Justification::centred);

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

    auto row = rightArea.reduced (4, 4);
    abBtnA.setBounds (row.removeFromLeft (22));
    row.removeFromLeft (2);
    abBtnB.setBounds (row.removeFromLeft (22));
    row.removeFromLeft (6);
    prevBtn.setBounds (row.removeFromLeft (28));
    nextBtn.setBounds (row.removeFromRight (28));
    presetLabel.setBounds (row);
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

void DirektHeader::syncABSnapshotsAfterPresetLoad()
{
    snapshotA = apvts.copyState();
    snapshotB = apvts.copyState();
    activeSlot = ABSlot::A;
    styleABButtons();
}

void DirektHeader::switchToSlot (ABSlot slot)
{
    if (slot == activeSlot)
        return;

    if (activeSlot == ABSlot::A)
        snapshotA = apvts.copyState();
    else
        snapshotB = apvts.copyState();

    apvts.replaceState (slot == ABSlot::A ? snapshotA : snapshotB);
    activeSlot = slot;
    styleABButtons();
}

void DirektHeader::styleABButtons()
{
    auto styleOne = [this] (juce::TextButton& b, bool isActive)
    {
        b.setColour (juce::TextButton::buttonColourId, isActive ? accent.withAlpha (0.35f) : Colours::bgHeader);
        b.setColour (juce::TextButton::textColourOffId, isActive ? Colours::textBright : Colours::textDim);
    };

    styleOne (abBtnA, activeSlot == ABSlot::A);
    styleOne (abBtnB, activeSlot == ABSlot::B);
}

} // namespace DirektDSP
