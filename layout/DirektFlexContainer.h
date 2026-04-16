#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include <vector>

#include "config/DirektDescriptors.h"

namespace DirektDSP
{

class DirektSection; // forward declaration for drag-reorder support

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
    void paintOverChildren (juce::Graphics& g) override;

    /**
     * @brief Enable or disable drag-to-reorder for DirektSection children.
     *
     * When enabled, each DirektSection child receives a drag handle in its
     * title bar and the container shows a drop-position indicator while the
     * user drags.  Only meaningful for Column-direction containers.
     */
    void enableDragReorder (bool enable);

private:
    Direction direction;

    struct ChildEntry
    {
        juce::Component* component = nullptr;
        SizeHint hint;
    };

    std::vector<ChildEntry> entries;

    // ── Drag-reorder state ────────────────────────────────────────────────────
    bool dragReorderEnabled = false;
    int draggedIndex = -1;
    int dropTargetIndex = -1;

    void setupSectionDragCallbacks (DirektSection* section);
    void clearSectionDragCallbacks (DirektSection* section);
    void handleDragMoved (DirektSection* section, juce::Point<int> screenPos);
    void handleDragEnded (DirektSection* section);
    int getDropIndexFromPosition (juce::Point<int> localPos) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektFlexContainer)
};

} // namespace DirektDSP
