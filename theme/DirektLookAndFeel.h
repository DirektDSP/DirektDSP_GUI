#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include "theme/DirektColours.h"

namespace DirektDSP
{

class DirektLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DirektLookAndFeel();
    ~DirektLookAndFeel() override = default;

    void setAccentColour (juce::Colour c) { accentColour = c; }
    juce::Colour getAccentColour() const { return accentColour; }

    // --- Rotary Slider ---
    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;

    // --- Toggle Button ---
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;

    // --- ComboBox ---
    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW,
                       int buttonH, juce::ComboBox&) override;

    // --- Label ---
    void drawLabel (juce::Graphics&, juce::Label&) override;

    // --- PopupMenu ---
    void drawPopupMenuBackground (juce::Graphics&, int width, int height) override;
    void drawPopupMenuItem (juce::Graphics&, const juce::Rectangle<int>& area, bool isSeparator, bool isActive,
                            bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text,
                            const juce::String& shortcutKeyText, const juce::Drawable* icon,
                            const juce::Colour* textColour) override;

private:
    juce::Colour accentColour{Colours::accentCyan};
};

} // namespace DirektDSP
