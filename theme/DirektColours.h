#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace DirektDSP
{
    namespace Colours
    {
        // --- Backgrounds ---
        inline const juce::Colour bgDark       { 0xff1a1a1e };
        inline const juce::Colour bgPanel      { 0xff232328 };
        inline const juce::Colour bgHeader     { 0xff15151a };
        inline const juce::Colour bgFooter     { 0xff15151a };
        inline const juce::Colour bgSection    { 0xff2a2a30 };

        // --- Text ---
        inline const juce::Colour textBright   { 0xffe8e8ec };
        inline const juce::Colour textDim      { 0xff8a8a94 };
        inline const juce::Colour textLabel    { 0xffb0b0b8 };

        // --- Knob ---
        inline const juce::Colour knobBg       { 0xff303038 };
        inline const juce::Colour knobTrack    { 0xff404048 };
        inline const juce::Colour knobPointer  { 0xffe8e8ec };

        // --- Per-plugin accents ---
        inline const juce::Colour accentCyan   { 0xff00d4ff };  // Pneumatica
        inline const juce::Colour accentRed    { 0xffff2244 };  // Hydraulica
        inline const juce::Colour accentOrange { 0xffff8c1a };  // Mechanica
        inline const juce::Colour accentBlue   { 0xff4488ff };  // Electrica

        // --- Misc ---
        inline const juce::Colour divider      { 0xff3a3a42 };
        inline const juce::Colour popupDim     { 0xaa000000 };
    }
}
