#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include "theme/DirektColours.h"

namespace DirektDSP
{

class DirektPopupPanel : public juce::Component
{
public:
    DirektPopupPanel();

    void show (const juce::String& title, juce::Component* content, int contentW, int contentH);
    void hide();

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;

private:
    juce::String titleText;
    juce::Component* contentComp = nullptr;
    int requestedW = 400, requestedH = 300;
    juce::TextButton closeBtn{"X"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektPopupPanel)
};

} // namespace DirektDSP
