#include "layout/DirektFlexContainer.h"

namespace DirektDSP
{

DirektFlexContainer::DirektFlexContainer (Direction dir) : direction (dir) {}

void DirektFlexContainer::addFlexChild (juce::Component* child, const SizeHint& hint)
{
    entries.push_back ({child, hint});
    addAndMakeVisible (child);
}

void DirektFlexContainer::resized()
{
    juce::FlexBox fb;
    fb.flexDirection = (direction == Direction::Row) ? juce::FlexBox::Direction::row : juce::FlexBox::Direction::column;
    fb.flexWrap = juce::FlexBox::Wrap::noWrap;
    fb.alignContent = juce::FlexBox::AlignContent::stretch;

    for (auto& entry : entries)
    {
        juce::FlexItem item (*entry.component);
        item.flexGrow = entry.hint.flex;
        item.flexShrink = 1.0f;

        if (entry.hint.minW > 0)
            item.minWidth = static_cast<float> (entry.hint.minW);
        if (entry.hint.minH > 0)
            item.minHeight = static_cast<float> (entry.hint.minH);
        if (entry.hint.prefW > 0)
            item.width = static_cast<float> (entry.hint.prefW);
        if (entry.hint.prefH > 0)
            item.height = static_cast<float> (entry.hint.prefH);

        fb.items.add (item);
    }

    fb.performLayout (getLocalBounds());
}

} // namespace DirektDSP
