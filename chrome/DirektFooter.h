#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include "theme/DirektColours.h"

namespace DirektDSP
{

class DirektFooter : public juce::Component
{
public:
    DirektFooter();
    void paint (juce::Graphics& g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektFooter)
};

} // namespace DirektDSP
