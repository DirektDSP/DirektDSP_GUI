#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "DirektColours.h"

namespace DirektDSP
{

class DirektLabel : public juce::Label
{
public:
    enum Style { Title, Section, Body };

    DirektLabel (const juce::String& text, Style style = Body);
};

} // namespace DirektDSP
