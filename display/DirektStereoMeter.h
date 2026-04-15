#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include <atomic>

#include "display/DirektMeter.h"
#include "theme/DirektColours.h"

namespace DirektDSP
{

/** A stereo meter pair (L + R) laid out side-by-side (vertical) or
 *  stacked top-to-bottom (horizontal orientation). */
class DirektStereoMeter : public juce::Component
{
public:
    struct Config
    {
        DirektMeter::Orientation orientation = DirektMeter::Orientation::Vertical;
        DirektMeter::Mode mode = DirektMeter::Mode::Normal;
        juce::Colour colour = Colours::accentCyan;
        juce::String label;
        bool peakHold = true;
        int refreshHz = 30;
        float rangeMinDb = -60.0f;
        float rangeMaxDb = 0.0f;
    };

    explicit DirektStereoMeter (const Config& cfg);

    void setSourceLeft (const std::atomic<float>* src);
    void setSourceRight (const std::atomic<float>* src);

    void resized() override;

private:
    DirektMeter meterL;
    DirektMeter meterR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektStereoMeter)
};

} // namespace DirektDSP
