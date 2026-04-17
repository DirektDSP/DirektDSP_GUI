#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include <atomic>

#include "theme/DirektColours.h"

namespace DirektDSP
{

/** A small LED indicator that lights red when the signal has clipped (>= 0 dBFS).
 *  Click to reset the clip latch. */
class DirektClipIndicator : public juce::Component, public juce::SettableTooltipClient, private juce::Timer
{
public:
    explicit DirektClipIndicator (int refreshHz = 30);

    void setSource (const std::atomic<float>* src);

    /**
     * @brief Store the source ID used for post-build binding via
     *        DirektBaseEditor::bindMeterSource.
     */
    void setSourceID (const juce::String& id);

    /**
     * @brief Bind a source by ID.  Returns true if the ID matched the stored
     *        sourceID and the source was connected.
     */
    bool tryBindSource (const juce::String& id, const std::atomic<float>* src);

    void resetClip();

    bool hasClipped() const { return clipped; }

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;

private:
    void timerCallback() override;

    juce::String sourceID;
    const std::atomic<float>* source = nullptr;
    bool clipped = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektClipIndicator)
};

} // namespace DirektDSP
