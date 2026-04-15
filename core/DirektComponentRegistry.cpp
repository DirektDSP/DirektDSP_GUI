#include "core/DirektComponentRegistry.h"

#include "controls/DirektComboBox.h"
#include "controls/DirektKnob.h"
#include "controls/DirektToggle.h"
#include "display/DirektClipIndicator.h"
#include "display/DirektLabel.h"
#include "display/DirektMeter.h"
#include "display/DirektStereoMeter.h"
#include "layout/DirektFlexContainer.h"
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

BuiltNode buildButtonNode (const ButtonDesc& desc, BuildContext& /*ctx*/)
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

DirektMeter::Mode toMeterMode (MeterMode m)
{
    switch (m)
    {
        case MeterMode::GainReduction:
            return DirektMeter::Mode::GainReduction;
        case MeterMode::Rms:
            return DirektMeter::Mode::Rms;
        case MeterMode::Lufs:
            return DirektMeter::Mode::Lufs;
        default:
            return DirektMeter::Mode::Normal;
    }
}

BuiltNode buildMeterNode (const MeterDesc& desc, BuildContext& ctx)
{
    DirektMeter::Config meterCfg;
    meterCfg.orientation = (desc.orientation == MeterOrientation::Horizontal) ? DirektMeter::Orientation::Horizontal
                                                                              : DirektMeter::Orientation::Vertical;
    meterCfg.mode = toMeterMode (desc.mode);
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

BuiltNode buildStereoMeterNode (const StereoMeterDesc& desc, BuildContext& ctx)
{
    DirektStereoMeter::Config cfg;
    cfg.orientation = (desc.orientation == MeterOrientation::Horizontal) ? DirektMeter::Orientation::Horizontal
                                                                         : DirektMeter::Orientation::Vertical;
    cfg.mode = toMeterMode (desc.mode);
    cfg.label = desc.label;
    cfg.colour = ctx.lookAndFeel.getAccentColour();

    auto stereoMeter = std::make_unique<DirektStereoMeter> (cfg);

    auto itL = ctx.meterSources.find (desc.sourceIDLeft);
    if (itL != ctx.meterSources.end())
    {
        stereoMeter->setSourceLeft (itL->second);
    }

    auto itR = ctx.meterSources.find (desc.sourceIDRight);
    if (itR != ctx.meterSources.end())
    {
        stereoMeter->setSourceRight (itR->second);
    }

    applyNodeProps (*stereoMeter, desc.props);
    return {std::move (stereoMeter), {}};
}

BuiltNode buildClipIndicatorNode (const ClipIndicatorDesc& desc, BuildContext& ctx)
{
    auto indicator = std::make_unique<DirektClipIndicator>();
    indicator->setTooltip (desc.tooltip.isNotEmpty() ? desc.tooltip : juce::String ("Clip indicator — click to reset"));

    auto it = ctx.meterSources.find (desc.sourceID);
    if (it != ctx.meterSources.end())
    {
        indicator->setSource (it->second);
    }

    applyNodeProps (*indicator, desc.props);
    return {std::move (indicator), {}};
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
namespace
{
// Visitor struct keeps each overload to one line, avoiding the cognitive
// complexity penalty of a large if-constexpr chain inside a single lambda.
struct NodeBuilder
{
    BuildContext* ctx;
    const DirektComponentRegistry* reg;

    BuiltNode operator() (const KnobDesc& d) const { return buildKnobNode (d, *ctx); }
    BuiltNode operator() (const ToggleDesc& d) const { return buildToggleNode (d, *ctx); }
    BuiltNode operator() (const ComboBoxDesc& d) const { return buildComboBoxNode (d, *ctx); }
    BuiltNode operator() (const SliderDesc& d) const { return buildSliderNode (d, *ctx); }
    BuiltNode operator() (const ButtonDesc& d) const { return buildButtonNode (d, *ctx); }
    BuiltNode operator() (const RadioGroupDesc& d) const { return buildRadioGroupNode (d, *ctx); }
    BuiltNode operator() (const MeterDesc& d) const { return buildMeterNode (d, *ctx); }
    BuiltNode operator() (const StereoMeterDesc& d) const { return buildStereoMeterNode (d, *ctx); }
    BuiltNode operator() (const ClipIndicatorDesc& d) const { return buildClipIndicatorNode (d, *ctx); }
    BuiltNode operator() (const LabelDesc& d) const { return buildLabelNode (d, *ctx); }
    BuiltNode operator() (const DividerDesc& d) const { return buildDividerNode (d, *ctx); }
    BuiltNode operator() (const SectionDesc& d) const { return buildSectionNode (d, *ctx, *reg); }
    BuiltNode operator() (const CustomDesc& d) const { return buildCustomNode (d, *ctx, reg->getCustomFactories()); }

    BuiltNode operator() (const HBoxDesc& d) const
    {
        return buildFlexNode (d.children, d.props, *ctx, *reg, DirektFlexContainer::Direction::Row);
    }

    BuiltNode operator() (const VBoxDesc& d) const
    {
        return buildFlexNode (d.children, d.props, *ctx, *reg, DirektFlexContainer::Direction::Column);
    }

    BuiltNode operator() (const TabPanelDesc& d) const { return buildTabPanelNode (d, *ctx, *reg); }

    BuiltNode operator() (const XYPadDesc& d) const
    {
        auto comp = std::make_unique<juce::Component>();
        applyNodeProps (*comp, d.props);
        return {std::move (comp), {}};
    }

    BuiltNode operator() (const SpacerDesc& d) const
    {
        auto spacer = std::make_unique<juce::Component>();
        applyNodeProps (*spacer, d.props);
        return {std::move (spacer), {}};
    }
};
} // namespace

BuiltNode DirektComponentRegistry::build (const NodeDescriptor& descriptor, BuildContext& ctx) const
{
    return std::visit (NodeBuilder{&ctx, this}, descriptor);
}

} // namespace DirektDSP
