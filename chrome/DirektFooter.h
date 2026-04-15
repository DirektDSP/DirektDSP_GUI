#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "theme/DirektColours.h"

namespace DirektDSP
{

/**
 * @brief Footer chrome with snapshot capture and morph controls.
 *
 * Hosts can wire callbacks to capture two snapshots (A/B) and morph between
 * them with a normalized slider value.
 */
class DirektFooter : public juce::Component
{
public:
    DirektFooter();

    /**
     * @brief Paints footer background, border and version label.
     * @param g JUCE graphics context.
     */
    void paint (juce::Graphics& g) override;
    /**
     * @brief Lays out snapshot buttons and morph slider.
     */
    void resized() override;

    /**
     * @brief Updates button enabled state for available snapshots.
     * @param hasSnapshotA True when snapshot A exists.
     * @param hasSnapshotB True when snapshot B exists.
     */
    void setSnapshotAvailability (bool hasSnapshotA, bool hasSnapshotB);

    /** @brief Called when user clicks snapshot A capture button. */
    std::function<void()> onCaptureSnapshotA;
    /** @brief Called when user clicks snapshot B capture button. */
    std::function<void()> onCaptureSnapshotB;
    /**
     * @brief Called when morph slider value changes.
     * @param morphValue Normalized value in range [0.0, 1.0].
     */
    std::function<void (float morphValue)> onMorphChanged;

private:
    juce::TextButton captureAButton{"A"};
    juce::TextButton captureBButton{"B"};
    juce::Slider morphSlider;
    juce::Label morphLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektFooter)
};

} // namespace DirektDSP
