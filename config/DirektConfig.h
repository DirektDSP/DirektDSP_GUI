#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include "theme/DirektColours.h"

namespace DirektDSP
{

struct PluginConfig
{
    juce::String pluginName;
    juce::Colour accentColour = Colours::accentCyan;
    float aspectRatio = 1.5f;
    int defaultWidth = 800;
    int minWidth = 400;
    int maxWidth = 1600;
    bool showHeader = true;
    bool showFooter = true;
    bool resizable = true;
    bool showTooltips = true;
};

} // namespace DirektDSP
