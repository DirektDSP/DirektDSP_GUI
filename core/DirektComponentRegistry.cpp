#include "core/DirektComponentRegistry.h"

#include "controls/DirektComboBox.h"
#include "controls/DirektKnob.h"
#include "controls/DirektMacroKnob.h"
#include "controls/DirektToggle.h"
#include "display/DirektLabel.h"
#include "display/DirektMeter.h"
#include "layout/DirektFlexContainer.h"
#include "layout/DirektModuleBypassSoloStrip.h"
#include "layout/DirektSection.h"
#include "layout/DirektTabPanel.h"
#include "theme/DirektColours.h"
#include "theme/DirektLookAndFeel.h"

namespace DirektDSP
{

DirektComponentRegistry& DirektComponentRegistry::instance()
{
    static DirektComponentRegistry reg;
    return reg;
}

void DirektComponentRegistry::registerCustom (const juce::String& typeKey, CustomFactory factory)
{
    customFactories[typeKey] = std::move (factory);
}

// ── Per-type builder helpers ───────────────────────────────────────────────
namespace
{
void applyNodeProps (juce::Component& comp, const NodeProps& props)
{
    if (props.id.isNotEmpty())
    {
        comp.setComponentID (props.id);
    }
    comp.setVisible (props.visible);
}

BuiltNode buildKnobNode (const KnobDesc& desc, BuildContext& ctx)
{
    auto knob = std::make_unique<DirektKnob> (ctx.apvts, desc.paramID, desc.label);
    if (desc.tooltip.isNotEmpty())
    {
        knob->getSlider().setTooltip (desc.tooltip);
    }
    applyNodeProps (*knob, desc.props);
    return {std::move (knob), {}};
}

BuiltNode buildToggleNode (const ToggleDesc& desc, BuildContext& ctx)
{
    auto toggle = std::make_unique<DirektToggle> (ctx.apvts, desc.paramID, desc.label);
    if (desc.tooltip.isNotEmpty())
    {
        toggle->getButton().setTooltip (desc.tooltip);
    }
    applyNodeProps (*toggle, desc.props);
    return {std::move (toggle), {}};
}

BuiltNode buildComboBoxNode (const ComboBoxDesc& desc, BuildContext& ctx)
{
    auto combo = std::make_unique<DirektComboBox> (ctx.apvts, desc.paramID, desc.label);
    if (desc.tooltip.isNotEmpty())
    {
        combo->getComboBox().setTooltip (desc.tooltip);
    }
    applyNodeProps (*combo, desc.props);
    return {std::move (combo), {}};
}

BuiltNode buildMacroKnobNode (const MacroKnobDesc& desc, BuildContext& ctx)
{
    auto macroKnob = std::make_unique<DirektMacroKnob> (ctx.apvts, desc.paramID, desc.label, desc.targets);
    if (desc.tooltip.isNotEmpty())
    {
        macroKnob->getSlider().setTooltip (desc.tooltip);
    }
    applyNodeProps (*macroKnob, desc.props);
    return {std::move (macroKnob), {}};
}

BuiltNode buildSliderNode (const SliderDesc& desc, BuildContext& ctx)
{
    auto knob = std::make_unique<DirektKnob> (ctx.apvts, desc.paramID, desc.label);
    if (desc.tooltip.isNotEmpty())
    {
        knob->getSlider().setTooltip (desc.tooltip);
    }
    if (desc.horizontal)
    {
        knob->getSlider().setSliderStyle (juce::Slider::LinearHorizontal);
    }
    applyNodeProps (*knob, desc.props);
    return {std::move (knob), {}};
}

BuiltNode buildButtonNode (const ButtonDesc& desc, BuildContext& ctx)
{
    auto btn = std::make_unique<juce::TextButton> (desc.label);
    if (desc.tooltip.isNotEmpty())
    {
        btn->setTooltip (desc.tooltip);
    }
    applyNodeProps (*btn, desc.props);
    return {std::move (btn), {}};
}

BuiltNode buildRadioGroupNode (const RadioGroupDesc& desc, BuildContext& /*ctx*/)
{
    auto container = std::make_unique<DirektFlexContainer> (DirektFlexContainer::Direction::Column);
    applyNodeProps (*container, desc.props);
    std::vector<std::unique_ptr<juce::Component>> owned;
    for (size_t i = 0; i < desc.options.size(); ++i)
    {
        auto btn = std::make_unique<juce::ToggleButton> (desc.options[static_cast<int> (i)]);
        container->addFlexChild (btn.get(), desc.props.size);
        owned.push_back (std::move (btn));
    }
    return {std::move (container), std::move (owned)};
}

BuiltNode buildMeterNode (const MeterDesc& desc, BuildContext& ctx)
{
    DirektMeter::Config meterCfg;
    meterCfg.orientation = (desc.orientation == MeterOrientation::Horizontal) ? DirektMeter::Orientation::Horizontal
                                                                              : DirektMeter::Orientation::Vertical;
    meterCfg.mode =
        (desc.mode == MeterMode::GainReduction) ? DirektMeter::Mode::GainReduction : DirektMeter::Mode::Normal;
    meterCfg.label = desc.label;
    meterCfg.colour = ctx.lookAndFeel.getAccentColour();

    auto meter = std::make_unique<DirektMeter> (meterCfg);

    auto it = ctx.meterSources.find (desc.sourceID);
    if (it != ctx.meterSources.end())
    {
        meter->setSource (it->second);
    }

    applyNodeProps (*meter, desc.props);
    return {std::move (meter), {}};
}

BuiltNode buildLabelNode (const LabelDesc& desc, BuildContext& /*ctx*/)
{
    DirektLabel::Style labelStyle = DirektLabel::Body;
    if (desc.style == "title")
    {
        labelStyle = DirektLabel::Title;
    }
    else if (desc.style == "section")
    {
        labelStyle = DirektLabel::Section;
    }
    auto lbl = std::make_unique<DirektLabel> (desc.text, labelStyle);
    applyNodeProps (*lbl, desc.props);
    return {std::move (lbl), {}};
}

BuiltNode buildDividerNode (const DividerDesc& desc, BuildContext& /*ctx*/)
{
    struct DividerComp : public juce::Component
    {
        void paint (juce::Graphics& g) override
        {
            g.setColour (Colours::divider);
            g.fillRect (getLocalBounds());
        }
    };
    auto div = std::make_unique<DividerComp>();
    div->setSize (desc.horizontal ? 0 : 1, desc.horizontal ? 1 : 0);
    applyNodeProps (*div, desc.props);
    return {std::move (div), {}};
}

// Helpers for container nodes that recurse via the registry
BuiltNode buildSectionNode (const SectionDesc& desc, BuildContext& ctx, const DirektComponentRegistry& reg)
{
    auto section = std::make_unique<DirektSection> (desc.title, desc.columns);
    applyNodeProps (*section, desc.props);
    if (auto strip =
            DirektModuleBypassSoloStrip::tryCreate (ctx.apvts, desc.bypassParamID, desc.soloParamID, desc.bypassLabel,
                                                    desc.soloLabel, desc.bypassTooltip, desc.soloTooltip))
    {
        section->setTitleBarAccessory (std::move (strip));
    }
    std::vector<std::unique_ptr<juce::Component>> owned;
    for (const auto& childNode : desc.children)
    {
        if (!childNode)
        {
            continue;
        }
        auto built = reg.build (*childNode, ctx);
        section->addControl (built.component.get());
        owned.push_back (std::move (built.component));
        for (auto& oc : built.ownedChildren)
        {
            owned.push_back (std::move (oc));
        }
    }
    return {std::move (section), std::move (owned)};
}

BuiltNode buildFlexNode (const std::vector<Node>& children, const NodeProps& props, BuildContext& ctx,
                         const DirektComponentRegistry& reg, DirektFlexContainer::Direction dir)
{
    auto container = std::make_unique<DirektFlexContainer> (dir);
    applyNodeProps (*container, props);
    std::vector<std::unique_ptr<juce::Component>> owned;
    for (const auto& childNode : children)
    {
        if (!childNode)
        {
            continue;
        }
        auto childSize = std::visit ([] (const auto& d) { return d.props.size; }, *childNode);
        auto built = reg.build (*childNode, ctx);
        container->addFlexChild (built.component.get(), childSize);
        owned.push_back (std::move (built.component));
        for (auto& oc : built.ownedChildren)
        {
            owned.push_back (std::move (oc));
        }
    }
    return {std::move (container), std::move (owned)};
}

BuiltNode buildTabPanelNode (const TabPanelDesc& desc, BuildContext& ctx, const DirektComponentRegistry& reg)
{
    auto tabPanel = std::make_unique<DirektTabPanel> (ctx.lookAndFeel.getAccentColour());
    applyNodeProps (*tabPanel, desc.props);
    std::vector<std::unique_ptr<juce::Component>> owned;
    for (const auto& tab : desc.tabs)
    {
        if (tab.content)
        {
            auto built = reg.build (*tab.content, ctx);
            tabPanel->addTab (tab.label, built.component.get());
            owned.push_back (std::move (built.component));
            for (auto& oc : built.ownedChildren)
            {
                owned.push_back (std::move (oc));
            }
        }
    }
    return {std::move (tabPanel), std::move (owned)};
}

BuiltNode buildCustomNode (const CustomDesc& desc, BuildContext& ctx,
                           const std::map<juce::String, DirektComponentRegistry::CustomFactory>& factories)
{
    auto it = factories.find (desc.typeKey);
    if (it != factories.end())
    {
        return it->second (desc, ctx);
    }
    auto comp = std::make_unique<juce::Component>();
    applyNodeProps (*comp, desc.props);
    return {std::move (comp), {}};
}
} // namespace

// ── Dispatch ──────────────────────────────────────────────────────────────
BuiltNode DirektComponentRegistry::build (const NodeDescriptor& descriptor, BuildContext& ctx) const
{
    return std::visit (
        [&] (const auto& desc) -> BuiltNode
        {
            using T = std::decay_t<decltype (desc)>;
            if constexpr (std::is_same_v<T, KnobDesc>)
                return buildKnobNode (desc, ctx);
            else if constexpr (std::is_same_v<T, ToggleDesc>)
                return buildToggleNode (desc, ctx);
            else if constexpr (std::is_same_v<T, ComboBoxDesc>)
                return buildComboBoxNode (desc, ctx);
            else if constexpr (std::is_same_v<T, MacroKnobDesc>)
                return buildMacroKnobNode (desc, ctx);
            else if constexpr (std::is_same_v<T, SliderDesc>)
                return buildSliderNode (desc, ctx);
            else if constexpr (std::is_same_v<T, ButtonDesc>)
                return buildButtonNode (desc, ctx);
            else if constexpr (std::is_same_v<T, RadioGroupDesc>)
                return buildRadioGroupNode (desc, ctx);
            else if constexpr (std::is_same_v<T, XYPadDesc>)
            {
                auto comp = std::make_unique<juce::Component>();
                applyNodeProps (*comp, desc.props);
                return {std::move (comp), {}};
            }
            else if constexpr (std::is_same_v<T, MeterDesc>)
                return buildMeterNode (desc, ctx);
            else if constexpr (std::is_same_v<T, LabelDesc>)
                return buildLabelNode (desc, ctx);
            else if constexpr (std::is_same_v<T, SpacerDesc>)
            {
                auto spacer = std::make_unique<juce::Component>();
                applyNodeProps (*spacer, desc.props);
                return {std::move (spacer), {}};
            }
            else if constexpr (std::is_same_v<T, DividerDesc>)
                return buildDividerNode (desc, ctx);
            else if constexpr (std::is_same_v<T, SectionDesc>)
                return buildSectionNode (desc, ctx, *this);
            else if constexpr (std::is_same_v<T, HBoxDesc>)
                return buildFlexNode (desc.children, desc.props, ctx, *this, DirektFlexContainer::Direction::Row);
            else if constexpr (std::is_same_v<T, VBoxDesc>)
                return buildFlexNode (desc.children, desc.props, ctx, *this, DirektFlexContainer::Direction::Column);
            else if constexpr (std::is_same_v<T, TabPanelDesc>)
                return buildTabPanelNode (desc, ctx, *this);
            else if constexpr (std::is_same_v<T, CustomDesc>)
                return buildCustomNode (desc, ctx, customFactories);
            else
            {
                auto comp = std::make_unique<juce::Component>();
                return {std::move (comp), {}};
            }
        },
        descriptor);
}

} // namespace DirektDSP
