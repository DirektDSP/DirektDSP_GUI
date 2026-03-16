#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "DirektSection.h"
#include "DirektKnob.h"
#include "DirektToggle.h"
#include "DirektComboBox.h"

namespace DirektDSP
{

enum class ControlType { Knob, Toggle, ComboBox };

struct ParamSlot
{
    juce::String paramID;
    juce::String label;
    ControlType  type = ControlType::Knob;
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
    static std::vector<BuiltSection> buildSections (
        juce::AudioProcessorValueTreeState& apvts,
        const std::vector<SectionDescriptor>& descriptors);
};

} // namespace DirektDSP
