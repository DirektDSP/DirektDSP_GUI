#include "core/DirektAutoLayout.h"

#include "config/DirektDescriptorHelpers.h"

namespace DirektDSP
{
namespace
{
/**
 * @brief Creates toggle control for module bypass/solo binding.
 *
 * @param apvts Processor parameter tree used for attachment.
 * @param paramID Parameter ID for created toggle.
 * @param label Label shown on toggle.
 * @param tooltip Optional tooltip text.
 * @return Toggle component bound to given parameter.
 */
std::unique_ptr<juce::Component> makeModuleToggleControl (juce::AudioProcessorValueTreeState& apvts,
                                                          const juce::String& paramID, const juce::String& label,
                                                          const juce::String& tooltip)
{
    auto control = std::make_unique<DirektToggle> (apvts, paramID, label);

    if (tooltip.isNotEmpty())
    {
        control->getButton().setTooltip (tooltip);
    }

    return control;
}

/**
 * @brief Injects module bypass/solo controls into built section when configured.
 *
 * @param apvts Processor parameter tree used for attachments.
 * @param sectionDescriptor Section definition being converted.
 * @param builtSection Mutable section being populated with controls.
 */
void appendModuleIsolationControls (juce::AudioProcessorValueTreeState& apvts,
                                    const SectionDescriptor& sectionDescriptor, BuiltSection& builtSection)
{
    if (sectionDescriptor.moduleControls.bypassParamID.isNotEmpty())
    {
        auto control = makeModuleToggleControl (apvts, sectionDescriptor.moduleControls.bypassParamID,
                                                sectionDescriptor.moduleControls.bypassLabel,
                                                sectionDescriptor.moduleControls.bypassTooltip);
        builtSection.section->addControl (control.get());
        builtSection.controls.push_back (std::move (control));
    }

    if (sectionDescriptor.moduleControls.soloParamID.isNotEmpty())
    {
        auto control = makeModuleToggleControl (apvts, sectionDescriptor.moduleControls.soloParamID,
                                                sectionDescriptor.moduleControls.soloLabel,
                                                sectionDescriptor.moduleControls.soloTooltip);
        builtSection.section->addControl (control.get());
        builtSection.controls.push_back (std::move (control));
    }
}

/**
 * @brief Appends config-driven toggle descriptors for module bypass/solo bindings.
 *
 * @param sectionDescriptor Section definition being converted.
 * @param childNodes Mutable descriptor list for section children.
 */
void appendModuleIsolationNodes (const SectionDescriptor& sectionDescriptor, std::vector<Node>& childNodes)
{
    if (sectionDescriptor.moduleControls.bypassParamID.isNotEmpty())
    {
        childNodes.push_back (node (ToggleDesc{{},
                                               sectionDescriptor.moduleControls.bypassParamID,
                                               sectionDescriptor.moduleControls.bypassLabel,
                                               sectionDescriptor.moduleControls.bypassTooltip}));
    }

    if (sectionDescriptor.moduleControls.soloParamID.isNotEmpty())
    {
        childNodes.push_back (node (ToggleDesc{{},
                                               sectionDescriptor.moduleControls.soloParamID,
                                               sectionDescriptor.moduleControls.soloLabel,
                                               sectionDescriptor.moduleControls.soloTooltip}));
    }
}
} // namespace

std::vector<BuiltSection> DirektAutoLayout::buildSections (juce::AudioProcessorValueTreeState& apvts,
                                                           const std::vector<SectionDescriptor>& descriptors)
{
    std::vector<BuiltSection> result;

    for (const auto& desc : descriptors)
    {
        BuiltSection built;
        built.section = std::make_unique<DirektSection> (desc.title, desc.columns);
        appendModuleIsolationControls (apvts, desc, built);

        for (const auto& slot : desc.params)
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
        appendModuleIsolationNodes (desc, childNodes);

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
