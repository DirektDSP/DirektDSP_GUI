#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "layout/DirektSection.h"
#include "controls/DirektKnob.h"
#include "controls/DirektToggle.h"
#include "controls/DirektComboBox.h"
#include "config/DirektDescriptors.h"

namespace DirektDSP
{

enum class ControlType { Knob, Toggle, ComboBox };

struct ParamSlot
{
    juce::String paramID;
    juce::String label;
    ControlType  type = ControlType::Knob;
    juce::String tooltip;  // optional — empty = no tooltip
};

struct SectionDescriptor
{
    juce::String            title;
    std::vector<ParamSlot>  params;
    int                     columns = 0; // 0 = auto (one row)
};

struct BuiltSection
{
    std::unique_ptr<DirektSection>                  section;
    std::vector<std::unique_ptr<juce::Component>>   controls;
};

class DirektAutoLayout
{
public:
    // Legacy: build JUCE components from SectionDescriptors
    static std::vector<BuiltSection> buildSections (
        juce::AudioProcessorValueTreeState& apvts,
        const std::vector<SectionDescriptor>& descriptors);

    // Bridge: convert legacy SectionDescriptors to a NodeDescriptor tree
    static NodeDescriptor convertLegacySections (
        const std::vector<SectionDescriptor>& descriptors);
};

} // namespace DirektDSP
