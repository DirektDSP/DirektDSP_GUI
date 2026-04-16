#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace DirektDSP
{

enum class ThemeVariant
{
    Dark,
    Light,
    HighContrast
};

/** A complete colour palette for one visual theme.
 *  Use the static factory functions to obtain a pre-configured palette,
 *  or construct one manually for plugin-specific customisation. */
struct DirektTheme
{
    ThemeVariant variant = ThemeVariant::Dark;

    // ── Backgrounds ─────────────────────────────────────────────────────────
    juce::Colour bgDark{0xff1a1a1e};
    juce::Colour bgPanel{0xff232328};
    juce::Colour bgHeader{0xff15151a};
    juce::Colour bgFooter{0xff15151a};
    juce::Colour bgSection{0xff2a2a30};

    // ── Text ────────────────────────────────────────────────────────────────
    juce::Colour textBright{0xffe8e8ec};
    juce::Colour textDim{0xff8a8a94};
    juce::Colour textLabel{0xffb0b0b8};

    // ── Knob / slider ────────────────────────────────────────────────────────
    juce::Colour knobBg{0xff303038};
    juce::Colour knobTrack{0xff404048};
    juce::Colour knobPointer{0xffe8e8ec};

    // ── Misc ────────────────────────────────────────────────────────────────
    juce::Colour divider{0xff3a3a42};
    juce::Colour popupDim{0xaa000000};

    // ── Factories ────────────────────────────────────────────────────────────
    static DirektTheme dark();
    static DirektTheme light();
    static DirektTheme highContrast();
};

} // namespace DirektDSP
