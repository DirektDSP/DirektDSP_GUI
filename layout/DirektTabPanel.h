#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "theme/DirektColours.h"
#include <vector>

namespace DirektDSP
{

class DirektTabPanel : public juce::Component
{
public:
    explicit DirektTabPanel (juce::Colour accentColour);

    void addTab (const juce::String& label, juce::Component* content);
    void setActiveTab (int index);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    struct TabEntry
    {
        juce::String     label;
        juce::Component* content = nullptr;
    };

    juce::Colour accent;
    std::vector<TabEntry> tabs;
    std::vector<std::unique_ptr<juce::TextButton>> tabButtons;
    int activeIndex = 0;

    static constexpr int tabBarHeight = 28;

    void updateVisibility();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektTabPanel)
};

} // namespace DirektDSP
