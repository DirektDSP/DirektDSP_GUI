#include "layout/DirektSection.h"

namespace DirektDSP
{

DirektSection::DirektSection (const juce::String& t, int columns)
    : title (t), numColumns (columns)
{
}

void DirektSection::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Rounded rect background
    g.setColour (Colours::bgSection);
    g.fillRoundedRectangle (bounds, 6.0f);

    // Border
    g.setColour (Colours::divider);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 6.0f, 1.0f);

    // Title
    if (title.isNotEmpty())
    {
        auto titleArea = bounds.removeFromTop (static_cast<float>(titleHeight));
        g.setColour (Colours::textLabel);
        g.setFont (juce::Font (juce::FontOptions (12.0f).withStyle ("Bold")));
        g.drawText (title, titleArea.reduced (static_cast<float>(padding), 0.0f).toNearestInt(),
                    juce::Justification::centredLeft);
    }
}

void DirektSection::resized()
{
    if (controls.empty())
        return;

    auto bounds = getLocalBounds().reduced (padding);
    if (title.isNotEmpty())
        bounds.removeFromTop (titleHeight);

    int cols = numColumns > 0 ? numColumns : static_cast<int>(controls.size());
    int rows = (static_cast<int>(controls.size()) + cols - 1) / cols;
    auto cellW = bounds.getWidth() / cols;
    auto cellH = bounds.getHeight() / rows;

    for (int i = 0; i < static_cast<int>(controls.size()); ++i)
    {
        int col = i % cols;
        int row = i / cols;
        auto cell = juce::Rectangle<int> (bounds.getX() + col * cellW,
                                           bounds.getY() + row * cellH,
                                           cellW, cellH);
        controls[static_cast<size_t>(i)]->setBounds (cell.reduced (2));
    }
}

void DirektSection::addControl (juce::Component* control)
{
    controls.push_back (control);
    addAndMakeVisible (control);
}

} // namespace DirektDSP
