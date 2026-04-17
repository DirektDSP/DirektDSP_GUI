#include "controls/DirektABCompare.h"

namespace DirektDSP
{

DirektABCompare::DirektABCompare (juce::AudioProcessorValueTreeState& apvts_) : apvts (apvts_)
{
    snapshots[0] = apvts.copyState();
    snapshots[1] = apvts.copyState();

    btnA.setClickingTogglesState (false);
    btnB.setClickingTogglesState (false);
    btnCopy.setClickingTogglesState (false);

    btnA.onClick = [this] { onSlotClicked (0); };
    btnB.onClick = [this] { onSlotClicked (1); };
    btnCopy.onClick = [this] { onCopyClicked(); };

    addAndMakeVisible (btnA);
    addAndMakeVisible (btnB);
    addAndMakeVisible (btnCopy);

    updateButtonStates();
}

void DirektABCompare::onSlotClicked (int slot)
{
    if (slot == activeSlot)
        return;

    snapshots[activeSlot] = apvts.copyState();
    activeSlot = slot;
    apvts.replaceState (snapshots[activeSlot].createCopy());
    updateButtonStates();
}

void DirektABCompare::onCopyClicked()
{
    int const target = 1 - activeSlot;
    snapshots[target] = snapshots[activeSlot].createCopy();
}

void DirektABCompare::updateButtonStates()
{
    btnA.setToggleState (activeSlot == 0, juce::dontSendNotification);
    btnB.setToggleState (activeSlot == 1, juce::dontSendNotification);
    btnCopy.setButtonText (activeSlot == 0 ? "A\u2192B" : "B\u2192A");
}

void DirektABCompare::resized()
{
    auto bounds = getLocalBounds();
    const int slotW = bounds.getWidth() / 4;
    btnA.setBounds (bounds.removeFromLeft (slotW));
    btnB.setBounds (bounds.removeFromLeft (slotW));
    btnCopy.setBounds (bounds.removeFromLeft (slotW * 2));
}

} // namespace DirektDSP
