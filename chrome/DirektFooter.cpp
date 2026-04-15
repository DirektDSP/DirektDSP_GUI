#include "chrome/DirektFooter.h"

namespace DirektDSP
{

DirektFooter::DirektFooter() = default;

void DirektFooter::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (Colours::bgFooter);
    g.fillRect (bounds);

    // Top divider
    g.setColour (Colours::divider);
    g.fillRect (getLocalBounds().removeFromTop (1));

    g.setColour (Colours::textDim);
    g.setFont (juce::Font (juce::FontOptions (10.0F)));

#if defined(VERSION)
    juce::String versionStr = "v" + juce::String (VERSION);
#else
    juce::String const versionStr = "";
#endif

    g.drawText ("DirektDSP " + versionStr, bounds.toNearestInt(), juce::Justification::centred);
}

} // namespace DirektDSP
