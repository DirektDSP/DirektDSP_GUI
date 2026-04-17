#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <array>

namespace DirektDSP
{

/**
 * @brief A/B compare control that stores two APVTS snapshots and lets the user
 *        switch between them.
 *
 * Clicking the inactive slot saves the current APVTS state to the active slot
 * then recalls the target slot.  The arrow button copies the active snapshot
 * to the inactive slot without switching.
 */
class DirektABCompare : public juce::Component
{
public:
    explicit DirektABCompare (juce::AudioProcessorValueTreeState& apvts);

    void resized() override;

private:
    void onSlotClicked (int slot);
    void onCopyClicked();
    void updateButtonStates();

    juce::AudioProcessorValueTreeState& apvts;
    std::array<juce::ValueTree, 2> snapshots;
    int activeSlot = 0;

    juce::TextButton btnA{"A"};
    juce::TextButton btnB{"B"};
    juce::TextButton btnCopy{"A\u2192B"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektABCompare)
};

} // namespace DirektDSP
