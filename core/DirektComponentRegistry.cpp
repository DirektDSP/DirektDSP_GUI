#include "core/DirektComponentRegistry.h"
#include "controls/DirektKnob.h"
#include "controls/DirektToggle.h"
#include "controls/DirektComboBox.h"
#include "display/DirektMeter.h"
#include "display/DirektLabel.h"
#include "layout/DirektSection.h"
#include "layout/DirektFlexContainer.h"
#include "layout/DirektTabPanel.h"
#include "theme/DirektColours.h"

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

// Helper to apply common NodeProps to a built component
static void applyNodeProps (juce::Component& comp, const NodeProps& props)
{
    if (props.id.isNotEmpty())
        comp.setComponentID (props.id);
    comp.setVisible (props.visible);
}

// Helper to set tooltip on a component if it supports it
static void applyTooltip (juce::Component& comp, const juce::String& tooltip)
{
    if (tooltip.isNotEmpty())
        if (auto* ttc = dynamic_cast<juce::TooltipClient*> (&comp))
            juce::ignoreUnused (ttc); // Tooltips set on sub-components directly
}

BuiltNode DirektComponentRegistry::build (const NodeDescriptor& descriptor, BuildContext& ctx) const
{
    return std::visit ([&] (const auto& desc) -> BuiltNode
    {
        using T = std::decay_t<decltype (desc)>;

        if constexpr (std::is_same_v<T, KnobDesc>)
        {
            auto knob = std::make_unique<DirektKnob> (ctx.apvts, desc.paramID, desc.label);
            if (desc.tooltip.isNotEmpty())
                knob->getSlider().setTooltip (desc.tooltip);
            applyNodeProps (*knob, desc.props);
            return { std::move (knob), {} };
        }
        else if constexpr (std::is_same_v<T, ToggleDesc>)
        {
            auto toggle = std::make_unique<DirektToggle> (ctx.apvts, desc.paramID, desc.label);
            if (desc.tooltip.isNotEmpty())
                toggle->getButton().setTooltip (desc.tooltip);
            applyNodeProps (*toggle, desc.props);
            return { std::move (toggle), {} };
        }
        else if constexpr (std::is_same_v<T, ComboBoxDesc>)
        {
            auto combo = std::make_unique<DirektComboBox> (ctx.apvts, desc.paramID, desc.label);
            if (desc.tooltip.isNotEmpty())
                combo->getComboBox().setTooltip (desc.tooltip);
            applyNodeProps (*combo, desc.props);
            return { std::move (combo), {} };
        }
        else if constexpr (std::is_same_v<T, SliderDesc>)
        {
            // SliderDesc: reuse DirektKnob for now (future: dedicated slider component)
            auto knob = std::make_unique<DirektKnob> (ctx.apvts, desc.paramID, desc.label);
            if (desc.tooltip.isNotEmpty())
                knob->getSlider().setTooltip (desc.tooltip);
            if (desc.horizontal)
                knob->getSlider().setSliderStyle (juce::Slider::LinearHorizontal);
            applyNodeProps (*knob, desc.props);
            return { std::move (knob), {} };
        }
        else if constexpr (std::is_same_v<T, ButtonDesc>)
        {
            auto btn = std::make_unique<juce::TextButton> (desc.label);
            if (desc.tooltip.isNotEmpty())
                btn->setTooltip (desc.tooltip);
            applyNodeProps (*btn, desc.props);
            return { std::move (btn), {} };
        }
        else if constexpr (std::is_same_v<T, RadioGroupDesc>)
        {
            // RadioGroup: placeholder — render as a VBox of toggles
            auto container = std::make_unique<DirektFlexContainer> (DirektFlexContainer::Direction::Column);
            applyNodeProps (*container, desc.props);
            std::vector<std::unique_ptr<juce::Component>> owned;
            for (int i = 0; i < desc.options.size(); ++i)
            {
                auto btn = std::make_unique<juce::ToggleButton> (desc.options[i]);
                container->addFlexChild (btn.get(), desc.props.size);
                owned.push_back (std::move (btn));
            }
            return { std::move (container), std::move (owned) };
        }
        else if constexpr (std::is_same_v<T, XYPadDesc>)
        {
            // XYPad: placeholder — render as empty component with ID
            auto comp = std::make_unique<juce::Component>();
            applyNodeProps (*comp, desc.props);
            return { std::move (comp), {} };
        }
        else if constexpr (std::is_same_v<T, MeterDesc>)
        {
            DirektMeter::Config meterCfg;
            meterCfg.orientation = (desc.orientation == MeterOrientation::Horizontal)
                ? DirektMeter::Orientation::Horizontal
                : DirektMeter::Orientation::Vertical;
            meterCfg.mode = (desc.mode == MeterMode::GainReduction)
                ? DirektMeter::Mode::GainReduction
                : DirektMeter::Mode::Normal;
            meterCfg.label = desc.label;
            meterCfg.colour = ctx.lookAndFeel.getAccentColour();

            auto meter = std::make_unique<DirektMeter> (meterCfg);

            // Connect to source if available
            auto it = ctx.meterSources.find (desc.sourceID);
            if (it != ctx.meterSources.end())
                meter->setSource (it->second);

            applyNodeProps (*meter, desc.props);
            return { std::move (meter), {} };
        }
        else if constexpr (std::is_same_v<T, LabelDesc>)
        {
            DirektLabel::Style labelStyle = DirektLabel::Body;
            if (desc.style == "title")        labelStyle = DirektLabel::Title;
            else if (desc.style == "section") labelStyle = DirektLabel::Section;

            auto lbl = std::make_unique<DirektLabel> (desc.text, labelStyle);
            applyNodeProps (*lbl, desc.props);
            return { std::move (lbl), {} };
        }
        else if constexpr (std::is_same_v<T, SpacerDesc>)
        {
            auto spacer = std::make_unique<juce::Component>();
            applyNodeProps (*spacer, desc.props);
            return { std::move (spacer), {} };
        }
        else if constexpr (std::is_same_v<T, DividerDesc>)
        {
            // Divider: a thin coloured line component
            struct DividerComp : public juce::Component
            {
                void paint (juce::Graphics& g) override
                {
                    g.setColour (Colours::divider);
                    g.fillRect (getLocalBounds());
                }
            };
            auto div = std::make_unique<DividerComp>();
            if (desc.horizontal)
                div->setSize (0, 1);
            else
                div->setSize (1, 0);
            applyNodeProps (*div, desc.props);
            return { std::move (div), {} };
        }
        else if constexpr (std::is_same_v<T, SectionDesc>)
        {
            auto section = std::make_unique<DirektSection> (desc.title, desc.columns);
            applyNodeProps (*section, desc.props);

            std::vector<std::unique_ptr<juce::Component>> owned;
            for (auto& childNode : desc.children)
            {
                if (childNode)
                {
                    auto built = build (*childNode, ctx);
                    section->addControl (built.component.get());
                    owned.push_back (std::move (built.component));
                    for (auto& oc : built.ownedChildren)
                        owned.push_back (std::move (oc));
                }
            }
            return { std::move (section), std::move (owned) };
        }
        else if constexpr (std::is_same_v<T, HBoxDesc>)
        {
            auto container = std::make_unique<DirektFlexContainer> (DirektFlexContainer::Direction::Row);
            applyNodeProps (*container, desc.props);

            std::vector<std::unique_ptr<juce::Component>> owned;
            for (auto& childNode : desc.children)
            {
                if (childNode)
                {
                    // Get the child's SizeHint from its NodeProps
                    auto childSize = std::visit ([] (const auto& d) { return d.props.size; }, *childNode);
                    auto built = build (*childNode, ctx);
                    container->addFlexChild (built.component.get(), childSize);
                    owned.push_back (std::move (built.component));
                    for (auto& oc : built.ownedChildren)
                        owned.push_back (std::move (oc));
                }
            }
            return { std::move (container), std::move (owned) };
        }
        else if constexpr (std::is_same_v<T, VBoxDesc>)
        {
            auto container = std::make_unique<DirektFlexContainer> (DirektFlexContainer::Direction::Column);
            applyNodeProps (*container, desc.props);

            std::vector<std::unique_ptr<juce::Component>> owned;
            for (auto& childNode : desc.children)
            {
                if (childNode)
                {
                    auto childSize = std::visit ([] (const auto& d) { return d.props.size; }, *childNode);
                    auto built = build (*childNode, ctx);
                    container->addFlexChild (built.component.get(), childSize);
                    owned.push_back (std::move (built.component));
                    for (auto& oc : built.ownedChildren)
                        owned.push_back (std::move (oc));
                }
            }
            return { std::move (container), std::move (owned) };
        }
        else if constexpr (std::is_same_v<T, TabPanelDesc>)
        {
            auto tabPanel = std::make_unique<DirektTabPanel> (ctx.lookAndFeel.getAccentColour());
            applyNodeProps (*tabPanel, desc.props);

            std::vector<std::unique_ptr<juce::Component>> owned;
            for (auto& tab : desc.tabs)
            {
                if (tab.content)
                {
                    auto built = build (*tab.content, ctx);
                    tabPanel->addTab (tab.label, built.component.get());
                    owned.push_back (std::move (built.component));
                    for (auto& oc : built.ownedChildren)
                        owned.push_back (std::move (oc));
                }
            }
            return { std::move (tabPanel), std::move (owned) };
        }
        else if constexpr (std::is_same_v<T, CustomDesc>)
        {
            auto it = customFactories.find (desc.typeKey);
            if (it != customFactories.end())
                return it->second (desc, ctx);

            // Unknown custom type — render as empty placeholder
            auto comp = std::make_unique<juce::Component>();
            applyNodeProps (*comp, desc.props);
            return { std::move (comp), {} };
        }
        else
        {
            // Fallback (should never happen with exhaustive variant)
            auto comp = std::make_unique<juce::Component>();
            return { std::move (comp), {} };
        }
    }, descriptor);
}

} // namespace DirektDSP
