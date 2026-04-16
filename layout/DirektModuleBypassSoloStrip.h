#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <vector>

#include "controls/DirektToggle.h"

namespace DirektDSP
{

/**
 * @brief Compact bypass and solo toggles for a module section title bar.
 *
 * Each non-empty parameter ID gets a DirektToggle bound through APVTS.
 * Solo mutual exclusion and bypass DSP logic belong in the audio processor; this component is GUI-only.
 */
class DirektModuleBypassSoloStrip : public juce::Component
{
public:
    DirektModuleBypassSoloStrip (juce::AudioProcessorValueTreeState& apvts, const juce::String& bypassParamID,
                                 const juce::String& soloParamID, const juce::String& bypassLabel,
                                 const juce::String& soloLabel, const juce::String& bypassTooltip,
                                 const juce::String& soloTooltip);

    /**
     * @brief Sum of toggle widths plus gaps; used to reserve space beside the section title.
     */
    int getIdealWidth() const noexcept;

    void resized() override;

    /**
     * @brief Builds a strip when at least one of @p bypassParamID or @p soloParamID is non-empty; otherwise nullptr.
     */
    static std::unique_ptr<DirektModuleBypassSoloStrip> tryCreate (
        juce::AudioProcessorValueTreeState& apvts, const juce::String& bypassParamID, const juce::String& soloParamID,
        const juce::String& bypassLabel, const juce::String& soloLabel, const juce::String& bypassTooltip,
        const juce::String& soloTooltip);

private:
    std::vector<std::unique_ptr<DirektToggle>> toggles;

    static constexpr int kToggleMinWidth = 52;
    static constexpr int kGap = 4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektModuleBypassSoloStrip)
};

} // namespace DirektDSP
