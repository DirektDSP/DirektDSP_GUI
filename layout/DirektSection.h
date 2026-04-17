#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>
#include <memory>

#include "theme/DirektColours.h"

namespace DirektDSP
{

class DirektSection : public juce::Component
{
public:
    explicit DirektSection (juce::String title, int columns = 0);

    void paint (juce::Graphics& g) override;
    void resized() override;

    void addControl (juce::Component* control);
    void setColumns (int cols) { numColumns = cols; }
    const juce::String& getTitle() const { return title; }

    /**
     * @brief Optional right-aligned controls in the title row (e.g. bypass / solo strip).
     *
     * @param accessory Owned component; typically DirektModuleBypassSoloStrip.
     */
    void setTitleBarAccessory (std::unique_ptr<juce::Component> accessory);

    // ── Drag-to-reorder ───────────────────────────────────────────────────────

    /**
     * @brief Enable or disable the drag handle in the title bar.
     *
     * When enabled, the section fires @ref onDragMoved and @ref onDragEnded
     * as the user drags from the title area.  Typically called by the parent
     * DirektFlexContainer after enableDragReorder(true).
     */
    void setDraggable (bool enable);

    /** Fired continuously while the section is being dragged; receives current screen position. */
    std::function<void (juce::Point<int>)> onDragMoved;

    /** Fired when the drag gesture ends (mouse released). */
    std::function<void()> onDragEnded;

    void mouseMove (const juce::MouseEvent& e) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

private:
    juce::String title;
    int numColumns;
    juce::OwnedArray<juce::Component> dummyOwners; // doesn't own — just for tracking
    std::vector<juce::Component*> controls;
    std::unique_ptr<juce::Component> titleBarAccessory;
    int titleBarAccessoryWidth = 0;

    bool draggable = false;
    bool dragging = false;

    static constexpr int titleHeight = 22;
    static constexpr int padding = 6;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektSection)
};

} // namespace DirektDSP
