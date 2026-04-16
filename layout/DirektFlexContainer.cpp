#include "layout/DirektFlexContainer.h"

#include "layout/DirektSection.h"

namespace DirektDSP
{

DirektFlexContainer::DirektFlexContainer (Direction dir) : direction (dir) {}

void DirektFlexContainer::addFlexChild (juce::Component* child, const SizeHint& hint)
{
    entries.push_back ({child, hint});
    addAndMakeVisible (child);

    if (dragReorderEnabled)
    {
        if (auto* section = dynamic_cast<DirektSection*> (child))
        {
            setupSectionDragCallbacks (section);
        }
    }
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
        item.flexShrink = 1.0F;

        if (entry.hint.minW > 0)
        {
            item.minWidth = static_cast<float> (entry.hint.minW);
        }
        if (entry.hint.minH > 0)
        {
            item.minHeight = static_cast<float> (entry.hint.minH);
        }
        if (entry.hint.prefW > 0)
        {
            item.width = static_cast<float> (entry.hint.prefW);
        }
        if (entry.hint.prefH > 0)
        {
            item.height = static_cast<float> (entry.hint.prefH);
        }

        fb.items.add (item);
    }

    fb.performLayout (getLocalBounds());
}

void DirektFlexContainer::paintOverChildren (juce::Graphics& g)
{
    if (draggedIndex < 0 || dropTargetIndex < 0)
    {
        return;
    }

    int const n = static_cast<int> (entries.size());
    if (n == 0)
    {
        return;
    }

    // Determine the Y coordinate at which to draw the insertion indicator
    int lineY = 0;
    if (dropTargetIndex == 0)
    {
        lineY = entries[0].component->getY();
    }
    else if (dropTargetIndex >= n)
    {
        lineY = entries[static_cast<size_t> (n - 1)].component->getBottom();
    }
    else
    {
        lineY = entries[static_cast<size_t> (dropTargetIndex)].component->getY();
    }

    // Draw a horizontal accent bar at the drop position
    g.setColour (juce::Colours::white.withAlpha (0.65F));
    g.fillRect (4, lineY - 2, getWidth() - 8, 3);
}

// ── Drag-reorder implementation ───────────────────────────────────────────────

void DirektFlexContainer::enableDragReorder (bool enable)
{
    dragReorderEnabled = enable;

    for (auto& entry : entries)
    {
        if (auto* section = dynamic_cast<DirektSection*> (entry.component))
        {
            if (enable)
            {
                setupSectionDragCallbacks (section);
            }
            else
            {
                clearSectionDragCallbacks (section);
            }
        }
    }

    repaint();
}

void DirektFlexContainer::setupSectionDragCallbacks (DirektSection* section)
{
    section->setDraggable (true);

    section->onDragMoved = [this, section] (juce::Point<int> screenPos) { handleDragMoved (section, screenPos); };

    section->onDragEnded = [this, section]() { handleDragEnded (section); };
}

void DirektFlexContainer::clearSectionDragCallbacks (DirektSection* section)
{
    section->setDraggable (false);
    section->onDragMoved = nullptr;
    section->onDragEnded = nullptr;
}

void DirektFlexContainer::handleDragMoved (DirektSection* section, juce::Point<int> screenPos)
{
    draggedIndex = -1;

    for (int i = 0; i < static_cast<int> (entries.size()); ++i)
    {
        if (entries[static_cast<size_t> (i)].component == section)
        {
            draggedIndex = i;
            break;
        }
    }

    auto const localPos = getLocalPoint (nullptr, screenPos);
    dropTargetIndex = getDropIndexFromPosition (localPos);
    repaint();
}

void DirektFlexContainer::handleDragEnded (DirektSection* section)
{
    // Locate the dragged section's current index
    int fromIndex = -1;

    for (int i = 0; i < static_cast<int> (entries.size()); ++i)
    {
        if (entries[static_cast<size_t> (i)].component == section)
        {
            fromIndex = i;
            break;
        }
    }

    // Only reorder when there is a meaningful position change
    bool const validFrom = fromIndex >= 0;
    bool const validDrop = dropTargetIndex >= 0;
    bool const wouldMove = (dropTargetIndex != fromIndex) && (dropTargetIndex != fromIndex + 1);

    if (validFrom && validDrop && wouldMove)
    {
        auto entry = entries[static_cast<size_t> (fromIndex)];
        entries.erase (entries.begin() + fromIndex);

        // Adjust insertion index for the removed element
        int const insertAt = (fromIndex < dropTargetIndex) ? dropTargetIndex - 1 : dropTargetIndex;
        entries.insert (entries.begin() + insertAt, entry);

        resized();
    }

    draggedIndex = -1;
    dropTargetIndex = -1;
    repaint();
}

int DirektFlexContainer::getDropIndexFromPosition (juce::Point<int> localPos) const
{
    int result = static_cast<int> (entries.size()); // default: append at end

    for (int i = 0; i < static_cast<int> (entries.size()); ++i)
    {
        auto const bounds = entries[static_cast<size_t> (i)].component->getBounds();
        if (localPos.y < bounds.getCentreY())
        {
            result = i;
            break;
        }
    }

    return result;
}

} // namespace DirektDSP
