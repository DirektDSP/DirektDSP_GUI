#include "display/DirektStereoMeter.h"

namespace DirektDSP
{

DirektStereoMeter::DirektStereoMeter (const Config& cfg)
    : meterL (DirektMeter::Config{cfg.rangeMinDb, cfg.rangeMaxDb, cfg.orientation, cfg.mode, cfg.colour, "L",
                                  cfg.peakHold, cfg.refreshHz}),
      meterR (DirektMeter::Config{cfg.rangeMinDb, cfg.rangeMaxDb, cfg.orientation, cfg.mode, cfg.colour, "R",
                                  cfg.peakHold, cfg.refreshHz})
{
    addAndMakeVisible (meterL);
    addAndMakeVisible (meterR);
}

void DirektStereoMeter::setSourceLeft (const std::atomic<float>* src)
{
    meterL.setSource (src);
}

void DirektStereoMeter::setSourceRight (const std::atomic<float>* src)
{
    meterR.setSource (src);
}

void DirektStereoMeter::setSourceIDs (const juce::String& leftID, const juce::String& rightID)
{
    sourceIDLeft = leftID;
    sourceIDRight = rightID;
}

bool DirektStereoMeter::tryBindSource (const juce::String& id, const std::atomic<float>* src)
{
    if (id == sourceIDLeft)
    {
        meterL.setSource (src);
        return true;
    }
    if (id == sourceIDRight)
    {
        meterR.setSource (src);
        return true;
    }
    return false;
}

void DirektStereoMeter::resized()
{
    auto bounds = getLocalBounds();
    meterL.setBounds (bounds.removeFromLeft (bounds.getWidth() / 2).reduced (1, 0));
    meterR.setBounds (bounds.reduced (1, 0));
}

} // namespace DirektDSP
