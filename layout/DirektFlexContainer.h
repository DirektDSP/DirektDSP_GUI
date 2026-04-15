#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include <vector>

#include "config/DirektDescriptors.h"

namespace DirektDSP
{

class DirektFlexContainer : public juce::Component
{
public:
    enum class Direction
    {
        Row,
        Column
    };

    explicit DirektFlexContainer (Direction dir);

    void addFlexChild (juce::Component* child, const SizeHint& hint = {});
    void resized() override;

private:
    Direction direction;

    struct ChildEntry
    {
        juce::Component* component = nullptr;
        SizeHint hint;
    };

    std::vector<ChildEntry> entries;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektFlexContainer)
};

} // namespace DirektDSP
