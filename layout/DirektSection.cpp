#include "layout/DirektSection.h"

#include <utility>

#include "layout/DirektModuleBypassSoloStrip.h"

namespace DirektDSP
{

DirektSection::DirektSection (juce::String t, int columns) : title (std::move (t)), numColumns (columns) {}

void DirektSection::setTitleBarAccessory (std::unique_ptr<juce::Component> accessory)
{
    titleBarAccessory = std::move (accessory);
    if (titleBarAccessory != nullptr)
    {
        addAndMakeVisible (*titleBarAccessory);
    }
}

void DirektSection::setDraggable (bool enable)
{
    draggable = enable;
    repaint();
}

void DirektSection::mouseMove (const juce::MouseEvent& e)
{
    if (!draggable)
    {
        return;
    }

    if (e.getPosition().y < titleHeight + padding)
    {
        setMouseCursor (juce::MouseCursor::DraggingHandCursor);
    }
    else
    {
        setMouseCursor (juce::MouseCursor::NormalCursor);
    }
}

void DirektSection::mouseDown (const juce::MouseEvent& e)
{
    if (!draggable)
    {
        return;
    }

    if (e.getPosition().y < titleHeight + padding)
    {
        dragging = true;
        repaint();
    }
}

void DirektSection::mouseDrag (const juce::MouseEvent& e)
{
    if (!dragging)
    {
        return;
    }

    if (onDragMoved != nullptr)
    {
        onDragMoved (e.getScreenPosition());
    }
}

void DirektSection::mouseUp (const juce::MouseEvent& /*e*/)
{
    if (!dragging)
    {
        return;
    }

    dragging = false;

    if (onDragEnded != nullptr)
    {
        onDragEnded();
    }

    repaint();
}

void DirektSection::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Rounded rect background — brighter while being dragged
    g.setColour (dragging ? Colours::bgSection.brighter (0.12F) : Colours::bgSection);
    g.fillRoundedRectangle (bounds, 6.0F);

    // Border — bolder when dragged to give visual feedback
    g.setColour (dragging ? Colours::textLabel.withAlpha (0.55F) : Colours::divider);
    g.drawRoundedRectangle (bounds.reduced (0.5F), 6.0F, dragging ? 2.0F : 1.0F);

    bool const showTitleBar = title.isNotEmpty() || titleBarAccessory != nullptr;
    if (showTitleBar && title.isNotEmpty())
    {
        auto titleArea = bounds.removeFromTop (static_cast<float> (titleHeight));
        if (titleBarAccessoryWidth > 0)
        {
            titleArea.removeFromRight (static_cast<float> (titleBarAccessoryWidth));
        }

        // Drag handle: 6 dots arranged in a 2×3 grid on the left side of the title
        if (draggable)
        {
            float const handleX = static_cast<float> (padding) + 1.0F;
            float const handleStartY = titleArea.getY() + (titleArea.getHeight() - 8.0F) * 0.5F;
            float const dotAlpha = dragging ? 0.6F : 0.35F;
            g.setColour (Colours::textLabel.withAlpha (dotAlpha));
            for (int row = 0; row < 3; ++row)
            {
                for (int col = 0; col < 2; ++col)
                {
                    g.fillEllipse (handleX + static_cast<float> (col) * 4.0F,
                                   handleStartY + static_cast<float> (row) * 4.0F, 2.0F, 2.0F);
                }
            }
        }

        // Offset title text right to clear the drag handle
        float const leftInset = draggable ? static_cast<float> (padding + 12) : static_cast<float> (padding);
        auto textArea = titleArea.withTrimmedLeft (leftInset).withTrimmedRight (static_cast<float> (padding));

        g.setColour (Colours::textLabel);
        g.setFont (juce::Font (juce::FontOptions (12.0F).withStyle ("Bold")));
        g.drawText (title, textArea.toNearestInt(), juce::Justification::centredLeft);
    }
}

void DirektSection::resized()
{
    auto work = getLocalBounds().reduced (padding);
    titleBarAccessoryWidth = 0;

    bool const showTitleBar = title.isNotEmpty() || titleBarAccessory != nullptr;
    if (showTitleBar)
    {
        auto titleRow = work.removeFromTop (titleHeight);
        if (titleBarAccessory != nullptr)
        {
            int accW = 0;
            if (auto* strip = dynamic_cast<DirektModuleBypassSoloStrip*> (titleBarAccessory.get()))
            {
                accW = strip->getIdealWidth();
            }
            else
            {
                accW = juce::jmax (titleBarAccessory->getWidth(), 48);
            }

            accW = juce::jmin (accW, titleRow.getWidth());
            titleBarAccessoryWidth = accW;
            int const h = juce::jmax (1, titleRow.getHeight() - 4);
            int const y = titleRow.getY() + (titleRow.getHeight() - h) / 2;
            titleBarAccessory->setBounds (titleRow.getRight() - accW, y, accW, h);
        }
    }

    if (controls.empty())
    {
        return;
    }

    auto bounds = work;

    int const cols = numColumns > 0 ? numColumns : static_cast<int> (controls.size());
    int const rows = (static_cast<int> (controls.size()) + cols - 1) / cols;
    auto cellW = bounds.getWidth() / cols;
    auto cellH = bounds.getHeight() / rows;

    for (int i = 0; i < static_cast<int> (controls.size()); ++i)
    {
        int const col = i % cols;
        int const row = i / cols;
        auto cell = juce::Rectangle<int> (bounds.getX() + col * cellW, bounds.getY() + row * cellH, cellW, cellH);
        controls[static_cast<size_t> (i)]->setBounds (cell.reduced (2));
    }
}

void DirektSection::addControl (juce::Component* control)
{
    controls.push_back (control);
    addAndMakeVisible (control);
}

} // namespace DirektDSP
