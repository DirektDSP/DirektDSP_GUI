#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "config/DirektDescriptors.h"
#include "controls/DirektComboBox.h"
#include "controls/DirektKnob.h"
#include "controls/DirektToggle.h"
#include "layout/DirektSection.h"

namespace DirektDSP
{

enum class ControlType
{
    Knob,
    Toggle,
    ComboBox
};

struct ParamSlot
{
    juce::String paramID;
    juce::String label;
    ControlType type = ControlType::Knob;
    juce::String tooltip; // optional — empty = no tooltip
};

struct SectionDescriptor
{
    /**
     * @brief Optional module controls rendered before regular parameter controls.
     *
     * Use these bindings for per-module isolation controls. When param IDs are set,
     * the layout builder injects matching toggle controls into the section.
     */
    struct ModuleControlBinding
    {
        /** @brief Parameter ID for module bypass toggle. Empty string disables bypass control. */
        juce::String bypassParamID;
        /** @brief Parameter ID for module solo toggle. Empty string disables solo control. */
        juce::String soloParamID;
        /** @brief Label text shown on module bypass toggle. */
        juce::String bypassLabel = "Bypass";
        /** @brief Label text shown on module solo toggle. */
        juce::String soloLabel = "Solo";
        /** @brief Optional tooltip text for module bypass toggle. */
        juce::String bypassTooltip;
        /** @brief Optional tooltip text for module solo toggle. */
        juce::String soloTooltip;
    };

    juce::String title;
    std::vector<ParamSlot> params;
    int columns = 0; // 0 = auto (one row)
    ModuleControlBinding moduleControls;
};

struct BuiltSection
{
    std::unique_ptr<DirektSection> section;
    std::vector<std::unique_ptr<juce::Component>> controls;
};

class DirektAutoLayout
{
public:
    // Legacy: build JUCE components from SectionDescriptors
    static std::vector<BuiltSection> buildSections (juce::AudioProcessorValueTreeState& apvts,
                                                    const std::vector<SectionDescriptor>& descriptors);

    // Bridge: convert legacy SectionDescriptors to a NodeDescriptor tree
    static NodeDescriptor convertLegacySections (const std::vector<SectionDescriptor>& descriptors);
};

} // namespace DirektDSP
