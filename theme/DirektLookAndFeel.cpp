#include "theme/DirektLookAndFeel.h"

namespace DirektDSP
{

DirektLookAndFeel::DirektLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, Colours::bgDark);
    setColour (juce::Label::textColourId, Colours::textLabel);
    setColour (juce::ComboBox::backgroundColourId, Colours::bgSection);
    setColour (juce::ComboBox::textColourId, Colours::textBright);
    setColour (juce::ComboBox::outlineColourId, Colours::divider);
    setColour (juce::ComboBox::arrowColourId, Colours::textDim);
    setColour (juce::PopupMenu::backgroundColourId, Colours::bgPanel);
    setColour (juce::PopupMenu::textColourId, Colours::textBright);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, Colours::bgSection);
    setColour (juce::PopupMenu::highlightedTextColourId, Colours::textBright);
}

void DirektLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                           int x, int y, int width, int height,
                                           float sliderPos,
                                           float rotaryStartAngle, float rotaryEndAngle,
                                           juce::Slider& slider)
{
    juce::ignoreUnused (slider);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (4.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmax (2.0f, radius * 0.12f);
    auto arcRadius = radius - lineW * 0.5f;

    // Background circle
    g.setColour (Colours::knobBg);
    g.fillEllipse (rx, ry, rw, rw);

    // Track arc (full range)
    juce::Path bgArc;
    bgArc.addCentredArc (centreX, centreY, arcRadius, arcRadius, 0.0f,
                          rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (Colours::knobTrack);
    g.strokePath (bgArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));

    // Value arc
    if (sliderPos > 0.0f)
    {
        juce::Path valueArc;
        valueArc.addCentredArc (centreX, centreY, arcRadius, arcRadius, 0.0f,
                                 rotaryStartAngle, toAngle, true);
        g.setColour (accentColour);
        g.strokePath (valueArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved,
                                                       juce::PathStrokeType::rounded));
    }

    // Glow behind value arc
    if (sliderPos > 0.0f)
    {
        auto glowColour = accentColour.withAlpha (0.15f);
        juce::Path glowArc;
        glowArc.addCentredArc (centreX, centreY, arcRadius, arcRadius, 0.0f,
                                rotaryStartAngle, toAngle, true);
        g.setColour (glowColour);
        g.strokePath (glowArc, juce::PathStrokeType (lineW * 3.0f, juce::PathStrokeType::curved,
                                                      juce::PathStrokeType::rounded));
    }

    // Pointer line
    juce::Path pointer;
    auto pointerLength = radius * 0.6f;
    auto pointerThickness = juce::jmax (1.5f, radius * 0.08f);
    pointer.addRoundedRectangle (-pointerThickness * 0.5f, -pointerLength, pointerThickness, pointerLength, 1.0f);
    pointer.applyTransform (juce::AffineTransform::rotation (toAngle).translated (centreX, centreY));
    g.setColour (Colours::knobPointer);
    g.fillPath (pointer);

    // Center dot
    auto dotRadius = radius * 0.12f;
    g.setColour (Colours::knobPointer);
    g.fillEllipse (centreX - dotRadius, centreY - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
}

void DirektLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                           bool shouldDrawButtonAsHighlighted,
                                           bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto h = bounds.getHeight();
    auto indicatorSize = juce::jmin (h - 4.0f, 18.0f);
    auto indicatorBounds = juce::Rectangle<float> (bounds.getX() + 2.0f,
                                                    bounds.getCentreY() - indicatorSize * 0.5f,
                                                    indicatorSize, indicatorSize);

    // Indicator background
    g.setColour (Colours::knobBg);
    g.fillRoundedRectangle (indicatorBounds, 3.0f);

    // Indicator border
    g.setColour (shouldDrawButtonAsHighlighted ? Colours::textDim : Colours::divider);
    g.drawRoundedRectangle (indicatorBounds, 3.0f, 1.0f);

    if (button.getToggleState())
    {
        auto inner = indicatorBounds.reduced (3.0f);
        g.setColour (accentColour);
        g.fillRoundedRectangle (inner, 2.0f);

        // Glow
        g.setColour (accentColour.withAlpha (0.2f));
        g.fillRoundedRectangle (indicatorBounds.expanded (2.0f), 4.0f);
    }

    // Label text
    auto textBounds = bounds.withTrimmedLeft (indicatorSize + 8.0f);
    g.setColour (Colours::textLabel);
    g.setFont (juce::Font (juce::FontOptions (juce::jmin (h * 0.55f, 13.0f))));
    g.drawText (button.getButtonText(), textBounds.toNearestInt(), juce::Justification::centredLeft);
}

void DirektLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                       int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                       juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int> (0, 0, width, height).toFloat();
    g.setColour (Colours::bgSection);
    g.fillRoundedRectangle (bounds, 4.0f);
    g.setColour (box.isMouseOver() ? accentColour.withAlpha (0.5f) : Colours::divider);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 4.0f, 1.0f);

    // Arrow
    auto arrowZone = juce::Rectangle<float> (static_cast<float>(width) - 20.0f, 0.0f, 16.0f, static_cast<float>(height));
    juce::Path arrow;
    arrow.addTriangle (arrowZone.getCentreX() - 4.0f, arrowZone.getCentreY() - 2.0f,
                       arrowZone.getCentreX() + 4.0f, arrowZone.getCentreY() - 2.0f,
                       arrowZone.getCentreX(), arrowZone.getCentreY() + 3.0f);
    g.setColour (Colours::textDim);
    g.fillPath (arrow);
}

void DirektLookAndFeel::drawLabel (juce::Graphics& g, juce::Label& label)
{
    g.fillAll (label.findColour (juce::Label::backgroundColourId));

    if (! label.isBeingEdited())
    {
        auto textArea = label.getBorderSize().subtractedFrom (label.getLocalBounds());
        g.setColour (label.findColour (juce::Label::textColourId, true));
        g.setFont (label.getFont());
        g.drawFittedText (label.getText(), textArea, label.getJustificationType(),
                          juce::jmax (1, (int) ((float) textArea.getHeight() / label.getFont().getHeight())),
                          label.getMinimumHorizontalScale());
    }
}

void DirektLookAndFeel::drawPopupMenuBackground (juce::Graphics& g, int width, int height)
{
    g.fillAll (Colours::bgPanel);
    g.setColour (Colours::divider);
    g.drawRect (0, 0, width, height);
}

void DirektLookAndFeel::drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area,
                                            bool isSeparator, bool isActive, bool isHighlighted,
                                            bool isTicked, bool /*hasSubMenu*/,
                                            const juce::String& text, const juce::String& /*shortcutKeyText*/,
                                            const juce::Drawable* /*icon*/, const juce::Colour* /*textColour*/)
{
    if (isSeparator)
    {
        auto r = area.reduced (5, 0);
        r.removeFromTop (r.getHeight() / 2 - 1);
        g.setColour (Colours::divider);
        g.fillRect (r.removeFromTop (1));
        return;
    }

    if (isHighlighted && isActive)
    {
        g.setColour (Colours::bgSection);
        g.fillRect (area);
    }

    auto textColourToUse = isActive ? Colours::textBright : Colours::textDim;
    if (isTicked)
        textColourToUse = accentColour;

    g.setColour (textColourToUse);
    g.setFont (juce::Font (juce::FontOptions (juce::jmin (static_cast<float>(area.getHeight()) * 0.6f, 14.0f))));
    g.drawFittedText (text, area.reduced (10, 0), juce::Justification::centredLeft, 1);
}

} // namespace DirektDSP
