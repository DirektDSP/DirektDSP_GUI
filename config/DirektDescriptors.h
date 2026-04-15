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
    : std::variant<KnobDesc, ToggleDesc, ComboBoxDesc, SliderDesc, ButtonDesc, RadioGroupDesc, XYPadDesc, MeterDesc,
                   StereoMeterDesc, ClipIndicatorDesc, LabelDesc, SpacerDesc, DividerDesc, SectionDesc, HBoxDesc,
                   VBoxDesc, TabPanelDesc, CustomDesc>
{
    using variant::variant;
};

} // namespace DirektDSP
