#include "core/DirektAutoLayout.h"

#include "config/DirektDescriptorHelpers.h"

namespace DirektDSP
{

std::vector<BuiltSection> DirektAutoLayout::buildSections (juce::AudioProcessorValueTreeState& apvts,
                                                           const std::vector<SectionDescriptor>& descriptors,
                                                           DirektParameterHistory* parameterHistory)
{
    std::vector<BuiltSection> result;

    for (const auto& desc : descriptors)
    {
        BuiltSection built;
        built.section = std::make_unique<DirektSection> (desc.title, desc.columns);

        for (const auto& slot : desc.params)
        {
            std::unique_ptr<juce::Component> control;

            switch (slot.type)
            {
                case ControlType::Knob:
                    control = std::make_unique<DirektKnob> (apvts, slot.paramID, slot.label, parameterHistory);
                    break;
                case ControlType::Toggle:
                    control = std::make_unique<DirektToggle> (apvts, slot.paramID, slot.label, parameterHistory);
                    break;
                case ControlType::ComboBox:
                    control = std::make_unique<DirektComboBox> (apvts, slot.paramID, slot.label, parameterHistory);
                    break;
            }

            if (slot.tooltip.isNotEmpty())
            {
                if (auto* knob = dynamic_cast<DirektKnob*> (control.get()))
                {
                    knob->getSlider().setTooltip (slot.tooltip);
                }
                else if (auto* toggle = dynamic_cast<DirektToggle*> (control.get()))
                {
                    toggle->getButton().setTooltip (slot.tooltip);
                }
                else if (auto* combo = dynamic_cast<DirektComboBox*> (control.get()))
                {
                    combo->getComboBox().setTooltip (slot.tooltip);
                }
            }

            built.section->addControl (control.get());
            built.controls.push_back (std::move (control));
        }

        result.push_back (std::move (built));
    }

    return result;
}

NodeDescriptor DirektAutoLayout::convertLegacySections (const std::vector<SectionDescriptor>& descriptors)
{
    std::vector<Node> sectionNodes;

    for (const auto& desc : descriptors)
    {
        std::vector<Node> childNodes;

        for (const auto& slot : desc.params)
        {
            switch (slot.type)
            {
                case ControlType::Knob:
                    childNodes.push_back (node (KnobDesc{{}, slot.paramID, slot.label, slot.tooltip}));
                    break;
                case ControlType::Toggle:
                    childNodes.push_back (node (ToggleDesc{{}, slot.paramID, slot.label, slot.tooltip}));
                    break;
                case ControlType::ComboBox:
                    childNodes.push_back (node (ComboBoxDesc{{}, slot.paramID, slot.label, slot.tooltip}));
                    break;
            }
        }

        sectionNodes.push_back (node (SectionDesc{{}, desc.title, desc.columns, std::move (childNodes)}));
    }

    return VBoxDesc{{}, std::move (sectionNodes)};
}

} // namespace DirektDSP
