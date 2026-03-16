#include "DirektAutoLayout.h"

namespace DirektDSP
{

std::vector<BuiltSection> DirektAutoLayout::buildSections (
    juce::AudioProcessorValueTreeState& apvts,
    const std::vector<SectionDescriptor>& descriptors)
{
    std::vector<BuiltSection> result;

    for (auto& desc : descriptors)
    {
        BuiltSection built;
        built.section = std::make_unique<DirektSection> (desc.title, desc.columns);

        for (auto& slot : desc.params)
        {
            std::unique_ptr<juce::Component> control;

            switch (slot.type)
            {
                case ControlType::Knob:
                    control = std::make_unique<DirektKnob> (apvts, slot.paramID, slot.label);
                    break;
                case ControlType::Toggle:
                    control = std::make_unique<DirektToggle> (apvts, slot.paramID, slot.label);
                    break;
                case ControlType::ComboBox:
                    control = std::make_unique<DirektComboBox> (apvts, slot.paramID, slot.label);
                    break;
            }

            built.section->addControl (control.get());
            built.controls.push_back (std::move (control));
        }

        result.push_back (std::move (built));
    }

    return result;
}

} // namespace DirektDSP
