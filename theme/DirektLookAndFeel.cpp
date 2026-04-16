#include "theme/DirektLookAndFeel.h"

namespace DirektDSP
{

DirektLookAndFeel::DirektLookAndFeel()
{
    applyThemeColourIds();
}

void DirektLookAndFeel::setTheme (const DirektTheme& t)
{
    theme = t;
    applyThemeColourIds();
}

void DirektLookAndFeel::applyThemeColourIds()
{
    setColour (juce::ResizableWindow::backgroundColourId, theme.bgDark);
    setColour (juce::Label::textColourId, theme.textLabel);
    setColour (juce::ComboBox::backgroundColourId, theme.bgSection);
    setColour (juce::ComboBox::textColourId, theme.textBright);
    setColour (juce::ComboBox::outlineColourId, theme.divider);
    setColour (juce::ComboBox::arrowColourId, theme.textDim);
    setColour (juce::PopupMenu::backgroundColourId, theme.bgPanel);
    setColour (juce::PopupMenu::textColourId, theme.textBright);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, theme.bgSection);
    setColour (juce::PopupMenu::highlightedTextColourId, theme.textBright);
}

void DirektLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    juce::ignoreUnused (slider);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (4.0F);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0F;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0F;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmax (2.0F, radius * 0.12F);
    auto arcRadius = radius - lineW * 0.5F;

    // Background circle
    g.setColour (theme.knobBg);
    g.fillEllipse (rx, ry, rw, rw);

    // Track arc (full range)
    juce::Path bgArc;
    bgArc.addCentredArc (centreX, centreY, arcRadius, arcRadius, 0.0F, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (theme.knobTrack);
    g.strokePath (bgArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Value arc
    if (sliderPos > 0.0F)
    {
        juce::Path valueArc;
        valueArc.addCentredArc (centreX, centreY, arcRadius, arcRadius, 0.0F, rotaryStartAngle, toAngle, true);
        g.setColour (accentColour);
        g.strokePath (valueArc,
                      juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Glow behind value arc
    if (sliderPos > 0.0F)
    {
        auto glowColour = accentColour.withAlpha (0.15F);
        juce::Path glowArc;
        glowArc.addCentredArc (centreX, centreY, arcRadius, arcRadius, 0.0F, rotaryStartAngle, toAngle, true);
        g.setColour (glowColour);
        g.strokePath (glowArc,
                      juce::PathStrokeType (lineW * 3.0F, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Pointer line
    juce::Path pointer;
    auto pointerLength = radius * 0.6F;
    auto pointerThickness = juce::jmax (1.5F, radius * 0.08F);
    pointer.addRoundedRectangle (-pointerThickness * 0.5F, -pointerLength, pointerThickness, pointerLength, 1.0F);
    pointer.applyTransform (juce::AffineTransform::rotation (toAngle).translated (centreX, centreY));
    g.setColour (theme.knobPointer);
    g.fillPath (pointer);

    // Center dot
    auto dotRadius = radius * 0.12F;
    g.setColour (theme.knobPointer);
    g.fillEllipse (centreX - dotRadius, centreY - dotRadius, dotRadius * 2.0F, dotRadius * 2.0F);
}

void DirektLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                          bool shouldDrawButtonAsHighlighted, bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto h = bounds.getHeight();
    auto indicatorSize = juce::jmin (h - 4.0F, 18.0F);
    auto indicatorBounds = juce::Rectangle<float> (bounds.getX() + 2.0F, bounds.getCentreY() - indicatorSize * 0.5F,
                                                   indicatorSize, indicatorSize);

    // Indicator background
    g.setColour (theme.knobBg);
    g.fillRoundedRectangle (indicatorBounds, 3.0F);

    // Indicator border
    g.setColour (shouldDrawButtonAsHighlighted ? theme.textDim : theme.divider);
    g.drawRoundedRectangle (indicatorBounds, 3.0F, 1.0F);

    if (button.getToggleState())
    {
        auto inner = indicatorBounds.reduced (3.0F);
        g.setColour (accentColour);
        g.fillRoundedRectangle (inner, 2.0F);

        // Glow
        g.setColour (accentColour.withAlpha (0.2F));
        g.fillRoundedRectangle (indicatorBounds.expanded (2.0F), 4.0F);
    }

    // Label text
    auto textBounds = bounds.withTrimmedLeft (indicatorSize + 8.0F);
    g.setColour (theme.textLabel);
    g.setFont (juce::Font (juce::FontOptions (juce::jmin (h * 0.55F, 13.0F))));
    g.drawText (button.getButtonText(), textBounds.toNearestInt(), juce::Justification::centredLeft);
}

void DirektLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool /*isButtonDown*/, int /*buttonX*/,
                                      int /*buttonY*/, int /*buttonW*/, int /*buttonH*/, juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int> (0, 0, width, height).toFloat();
    g.setColour (theme.bgSection);
    g.fillRoundedRectangle (bounds, 4.0F);
    g.setColour (box.isMouseOver() ? accentColour.withAlpha (0.5F) : theme.divider);
    g.drawRoundedRectangle (bounds.reduced (0.5F), 4.0F, 1.0F);

    // Arrow
    auto arrowZone =
        juce::Rectangle<float> (static_cast<float> (width) - 20.0F, 0.0F, 16.0F, static_cast<float> (height));
    juce::Path arrow;
    arrow.addTriangle (arrowZone.getCentreX() - 4.0F, arrowZone.getCentreY() - 2.0F, arrowZone.getCentreX() + 4.0F,
                       arrowZone.getCentreY() - 2.0F, arrowZone.getCentreX(), arrowZone.getCentreY() + 3.0F);
    g.setColour (theme.textDim);
    g.fillPath (arrow);
}

void DirektLookAndFeel::drawLabel (juce::Graphics& g, juce::Label& label)
{
    g.fillAll (label.findColour (juce::Label::backgroundColourId));

    if (!label.isBeingEdited())
    {
        auto textArea = label.getBorderSize().subtractedFrom (label.getLocalBounds());
        g.setColour (label.findColour (juce::Label::textColourId, true));
        g.setFont (label.getFont());
        g.drawFittedText (label.getText(), textArea, label.getJustificationType(),
                          juce::jmax (1, (int)((float)textArea.getHeight() / label.getFont().getHeight())),
                          label.getMinimumHorizontalScale());
    }
}

void DirektLookAndFeel::drawPopupMenuBackground (juce::Graphics& g, int width, int height)
{
    g.fillAll (theme.bgPanel);
    g.setColour (theme.divider);
    g.drawRect (0, 0, width, height);
}

void DirektLookAndFeel::drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator,
                                           bool isActive, bool isHighlighted, bool isTicked, bool /*hasSubMenu*/,
                                           const juce::String& text, const juce::String& /*shortcutKeyText*/,
                                           const juce::Drawable* /*icon*/, const juce::Colour* /*textColour*/)
{
    if (isSeparator)
    {
        auto r = area.reduced (5, 0);
        r.removeFromTop (r.getHeight() / 2 - 1);
        g.setColour (theme.divider);
        g.fillRect (r.removeFromTop (1));
        return;
    }

    if (isHighlighted && isActive)
    {
        g.setColour (theme.bgSection);
        g.fillRect (area);
    }

    auto textColourToUse = isActive ? theme.textBright : theme.textDim;
    if (isTicked)
    {
        textColourToUse = accentColour;
    }

    g.setColour (textColourToUse);
    g.setFont (juce::Font (juce::FontOptions (juce::jmin (static_cast<float> (area.getHeight()) * 0.6F, 14.0F))));
    g.drawFittedText (text, area.reduced (10, 0), juce::Justification::centredLeft, 1);
}

} // namespace DirektDSP
