#include "theme/DirektTheme.h"

namespace DirektDSP
{

DirektTheme DirektTheme::dark()
{
    return DirektTheme{}; // default member values are the dark palette
}

DirektTheme DirektTheme::light()
{
    DirektTheme t;
    t.variant = ThemeVariant::Light;

    t.bgDark = juce::Colour{0xfff0f0f4};
    t.bgPanel = juce::Colour{0xffe8e8ec};
    t.bgHeader = juce::Colour{0xffdcdce4};
    t.bgFooter = juce::Colour{0xffdcdce4};
    t.bgSection = juce::Colour{0xffe0e0e8};

    t.textBright = juce::Colour{0xff1a1a1e};
    t.textDim = juce::Colour{0xff606068};
    t.textLabel = juce::Colour{0xff404048};

    t.knobBg = juce::Colour{0xffd8d8e0};
    t.knobTrack = juce::Colour{0xffc0c0c8};
    t.knobPointer = juce::Colour{0xff1a1a1e};

    t.divider = juce::Colour{0xffbcbcc8};
    t.popupDim = juce::Colour{0x88ffffff};

    return t;
}

DirektTheme DirektTheme::highContrast()
{
    DirektTheme t;
    t.variant = ThemeVariant::HighContrast;

    t.bgDark = juce::Colour{0xff000000};
    t.bgPanel = juce::Colour{0xff0a0a0a};
    t.bgHeader = juce::Colour{0xff000000};
    t.bgFooter = juce::Colour{0xff000000};
    t.bgSection = juce::Colour{0xff111111};

    t.textBright = juce::Colour{0xffffffff};
    t.textDim = juce::Colour{0xffcccccc};
    t.textLabel = juce::Colour{0xffdddddd};

    t.knobBg = juce::Colour{0xff1a1a1a};
    t.knobTrack = juce::Colour{0xff333333};
    t.knobPointer = juce::Colour{0xffffffff};

    t.divider = juce::Colour{0xffffffff};
    t.popupDim = juce::Colour{0xbb000000};

    return t;
}

} // namespace DirektDSP
