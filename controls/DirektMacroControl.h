#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <vector>

namespace DirektDSP
{

/**
 * @brief Describes one mapped parameter target for a macro control.
 */
struct MacroControlTarget
{
    juce::RangedAudioParameter* parameter = nullptr;
    float minNormalized = 0.0f;
    float maxNormalized = 1.0f;
    float curveExponent = 1.0f;
};

/**
 * @brief One macro knob that drives multiple parameters.
 *
 * The macro value is normalized from 0..1 and remapped per target with a
 * min/max range and optional power-curve exponent before values are pushed to
 * host-notifying parameter updates.
 */
class DirektMacroControl : public juce::Component
{
public:
    /**
     * @brief Creates a macro control with pre-resolved parameter targets.
     *
     * @param labelText Visible label shown under macro knob.
     * @param targets Target parameter mappings to drive.
     */
    DirektMacroControl (const juce::String& labelText, std::vector<MacroControlTarget> targets);

    void resized() override;

    /**
     * @brief Returns internal slider for optional custom styling.
     */
    juce::Slider& getSlider() { return slider; }

private:
    void applyMacroValue (float normalizedValue);

    juce::Slider slider;
    juce::Label label;
    std::vector<MacroControlTarget> targets;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektMacroControl)
};

} // namespace DirektDSP
