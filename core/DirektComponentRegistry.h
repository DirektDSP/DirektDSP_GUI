#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "config/DirektDescriptors.h"
#include "core/DirektBuildContext.h"

namespace DirektDSP
{

struct BuiltNode
{
    std::unique_ptr<juce::Component> component;
    std::vector<std::unique_ptr<juce::Component>> ownedChildren;
};

class DirektComponentRegistry
{
public:
    using CustomFactory = std::function<BuiltNode (const CustomDesc&, BuildContext&)>;

    static DirektComponentRegistry& instance();

    void registerCustom (const juce::String& typeKey, CustomFactory factory);

    BuiltNode build (const NodeDescriptor& descriptor, BuildContext& ctx) const;

private:
    DirektComponentRegistry() = default;

    std::map<juce::String, CustomFactory> customFactories;
};

} // namespace DirektDSP
