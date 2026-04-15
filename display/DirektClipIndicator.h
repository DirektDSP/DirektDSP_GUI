#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include <atomic>

#include "theme/DirektColours.h"

namespace DirektDSP
{

/** A small LED indicator that lights red when the signal has clipped (>= 0 dBFS).
 *  Click to reset the clip latch. */
class DirektClipIndicator : public juce::Component, private juce::Timer
{
public:
    explicit DirektClipIndicator (int refreshHz = 30);

    void setSource (const std::atomic<float>* src);
    void resetClip();

    bool hasClipped() const { return clipped; }

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;

private:
    void timerCallback() override;

    const std::atomic<float>* source = nullptr;
    bool clipped = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektClipIndicator)
};

} // namespace DirektDSP
