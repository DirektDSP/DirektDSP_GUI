#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

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

private:
    juce::String title;
    int numColumns;
    juce::OwnedArray<juce::Component> dummyOwners; // doesn't own — just for tracking
    std::vector<juce::Component*> controls;

    static constexpr int titleHeight = 22;
    static constexpr int padding = 6;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektSection)
};

} // namespace DirektDSP
