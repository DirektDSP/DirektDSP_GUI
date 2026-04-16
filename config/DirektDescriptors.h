#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <memory>
#include <variant>
#include <vector>

namespace DirektDSP
{

// ============================================================================
// Common properties on every node
// ============================================================================

struct SizeHint
{
    int minW = 0;
    int minH = 0;
    int prefW = 0;
    int prefH = 0;
    float flex = 1.0f;
};

struct NodeProps
{
    juce::String id;
    SizeHint size;
    int padding = 0;
    bool visible = true;
};

// ============================================================================
// Forward-declare NodeDescriptor for recursive containers
// ============================================================================

struct NodeDescriptor;
using Node = std::unique_ptr<NodeDescriptor>;

// ============================================================================
// Control descriptors
// ============================================================================

struct KnobDesc
{
    NodeProps props;
    juce::String paramID;
    juce::String label;
    juce::String tooltip;
};

struct ToggleDesc
{
    NodeProps props;
    juce::String paramID;
    juce::String label;
    juce::String tooltip;
};

struct ComboBoxDesc
{
    NodeProps props;
    juce::String paramID;
    juce::String label;
    juce::String tooltip;
};

/**
 * @brief Shaping for macro position before mapping into each target’s normalized range.
 */
enum class MacroCurve
{
    Linear,    ///< Uniform sweep from @ref MacroTarget::normMin to @ref MacroTarget::normMax.
    Quadratic, ///< Ease-in along the macro travel (slower change when macro is low).
    SquareRoot ///< Ease-out (stronger response when macro is low).
};

/**
 * @brief One APVTS parameter written by a @ref MacroKnobDesc knob.
 */
struct MacroTarget
{
    juce::String paramID;                  ///< Destination parameter ID in @c AudioProcessorValueTreeState.
    float normMin = 0.0f;                  ///< Normalized destination value when shaped macro position is 0.
    float normMax = 1.0f;                  ///< Normalized destination value when shaped macro position is 1.
    MacroCurve curve = MacroCurve::Linear; ///< Curve from macro 0..1 before min/max mapping.
};

/**
 * @brief One rotary macro control that drives multiple parameters with independent ranges/curves.
 */
struct MacroKnobDesc
{
    NodeProps props;
    juce::String paramID; ///< Macro parameter ID (automation and preset state for the knob).
    juce::String label;
    juce::String tooltip;
    std::vector<MacroTarget> targets;
};

struct SliderDesc
{
    NodeProps props;
    juce::String paramID;
    juce::String label;
    juce::String tooltip;
    bool horizontal = false;
};

struct ButtonDesc
{
    NodeProps props;
    juce::String label;
    juce::String tooltip;
    bool momentary = false;
};

struct RadioGroupDesc
{
    NodeProps props;
    juce::String paramID;
    juce::StringArray options;
    juce::String tooltip;
};

struct XYPadDesc
{
    NodeProps props;
    juce::String paramX;
    juce::String paramY;
    juce::String tooltip;
};

/**
 * @brief One macro mapping target.
 */
struct MacroTargetDesc
{
    juce::String paramID;
    float minNormalized = 0.0f;
    float maxNormalized = 1.0f;
    float curveExponent = 1.0f;
};

/**
 * @brief Descriptor for a macro control.
 */
struct MacroDesc
{
    NodeProps props;
    juce::String label;
    juce::String tooltip;
    std::vector<MacroTargetDesc> targets;
};

// ============================================================================
// Display descriptors
// ============================================================================

enum class MeterOrientation
{
    Vertical,
    Horizontal
};
enum class MeterMode
{
    Normal,
    GainReduction,
    Rms,
    Lufs
};

struct MeterDesc
{
    NodeProps props;
    juce::String sourceID;
    MeterOrientation orientation = MeterOrientation::Vertical;
    MeterMode mode = MeterMode::Normal;
    juce::String label;
};

struct StereoMeterDesc
{
    NodeProps props;
    juce::String sourceIDLeft;
    juce::String sourceIDRight;
    MeterOrientation orientation = MeterOrientation::Vertical;
    MeterMode mode = MeterMode::Normal;
    juce::String label;
};

struct ClipIndicatorDesc
{
    NodeProps props;
    juce::String sourceID;
    juce::String tooltip;
};

struct LabelDesc
{
    NodeProps props;
    juce::String text;
    float fontSize = 12.0f;
    juce::String style; // "title", "section", "body"
};

// ============================================================================
// Structural descriptors
// ============================================================================

struct SpacerDesc
{
    NodeProps props;
};

struct DividerDesc
{
    NodeProps props;
    bool horizontal = true;
};

// ============================================================================
// Container descriptors
// ============================================================================

struct SectionDesc
{
    NodeProps props;
    juce::String title;
    int columns = 0;
    std::vector<Node> children;

    /** @brief APVTS bool parameter ID for module bypass; empty omits the control. */
    juce::String bypassParamID;
    /** @brief APVTS bool parameter ID for module solo; empty omits the control. */
    juce::String soloParamID;
    /** @brief Label on the bypass toggle (short text recommended). */
    juce::String bypassLabel{"Bypass"};
    /** @brief Label on the solo toggle (short text recommended). */
    juce::String soloLabel{"Solo"};
    /** @brief Optional tooltip on the bypass toggle. */
    juce::String bypassTooltip;
    /** @brief Optional tooltip on the solo toggle. */
    juce::String soloTooltip;
};

struct HBoxDesc
{
    NodeProps props;
    std::vector<Node> children;
};

struct VBoxDesc
{
    NodeProps props;
    std::vector<Node> children;
};

struct TabDesc
{
    juce::String label;
    Node content;
};

struct TabPanelDesc
{
    NodeProps props;
    std::vector<TabDesc> tabs;
};

// ============================================================================
// Escape hatch for custom components
// ============================================================================

struct CustomDesc
{
    NodeProps props;
    juce::String typeKey;
    std::map<juce::String, juce::String> properties;
};

// ============================================================================
// NodeDescriptor — variant of all descriptor types
// ============================================================================

struct NodeDescriptor
    : std::variant<KnobDesc, ToggleDesc, ComboBoxDesc, MacroKnobDesc, MacroDesc, SliderDesc, ButtonDesc, RadioGroupDesc,
                   XYPadDesc, MeterDesc, StereoMeterDesc, ClipIndicatorDesc, LabelDesc, SpacerDesc, DividerDesc,
                   SectionDesc, HBoxDesc, VBoxDesc, TabPanelDesc, CustomDesc>
{
    using variant::variant;
};

} // namespace DirektDSP
