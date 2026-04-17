#include "display/DirektClipIndicator.h"

namespace DirektDSP
{

DirektClipIndicator::DirektClipIndicator (int refreshHz)
{
    startTimerHz (refreshHz);
}

void DirektClipIndicator::setSource (const std::atomic<float>* src)
{
    source = src;
}

void DirektClipIndicator::setSourceID (const juce::String& id)
{
    sourceID = id;
}

bool DirektClipIndicator::tryBindSource (const juce::String& id, const std::atomic<float>* src)
{
    if (id == sourceID)
    {
        setSource (src);
        return true;
    }
    return false;
}

void DirektClipIndicator::resetClip()
{
    clipped = false;
    repaint();
}

void DirektClipIndicator::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (1.0F);
    const juce::Colour ledColour = clipped ? Colours::accentRed : Colours::bgPanel;
    g.setColour (ledColour);
    g.fillEllipse (bounds);
    g.setColour (Colours::divider);
    g.drawEllipse (bounds, 1.0F);
}

void DirektClipIndicator::mouseDown (const juce::MouseEvent& /*e*/)
{
    resetClip();
}

void DirektClipIndicator::timerCallback()
{
    if (source == nullptr)
    {
        return;
    }

    const float val = source->load (std::memory_order_relaxed);
    if (val >= 0.0F && !clipped)
    {
        clipped = true;
        repaint();
    }
}

} // namespace DirektDSP
