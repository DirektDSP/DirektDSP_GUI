#include "layout/DirektModuleBypassSoloStrip.h"

namespace DirektDSP
{

DirektModuleBypassSoloStrip::DirektModuleBypassSoloStrip (
    juce::AudioProcessorValueTreeState& apvts, const juce::String& bypassParamID, const juce::String& soloParamID,
    const juce::String& bypassLabel, const juce::String& soloLabel, const juce::String& bypassTooltip,
    const juce::String& soloTooltip)
{
    if (bypassParamID.isNotEmpty())
    {
        juce::String const lab = bypassLabel.isNotEmpty() ? bypassLabel : juce::String ("Bypass");
        auto t = std::make_unique<DirektToggle> (apvts, bypassParamID, lab);
        if (bypassTooltip.isNotEmpty())
        {
            t->getButton().setTooltip (bypassTooltip);
        }
        addAndMakeVisible (*t);
        toggles.push_back (std::move (t));
    }

    if (soloParamID.isNotEmpty())
    {
        juce::String const lab = soloLabel.isNotEmpty() ? soloLabel : juce::String ("Solo");
        auto t = std::make_unique<DirektToggle> (apvts, soloParamID, lab);
        if (soloTooltip.isNotEmpty())
        {
            t->getButton().setTooltip (soloTooltip);
        }
        addAndMakeVisible (*t);
        toggles.push_back (std::move (t));
    }
}

int DirektModuleBypassSoloStrip::getIdealWidth() const noexcept
{
    int const n = static_cast<int> (toggles.size());
    if (n == 0)
    {
        return 0;
    }
    return n * kToggleMinWidth + (n > 1 ? kGap : 0);
}

void DirektModuleBypassSoloStrip::resized()
{
    auto r = getLocalBounds();
    int const n = static_cast<int> (toggles.size());
    if (n == 0)
    {
        return;
    }

    int const totalGap = (n - 1) * kGap;
    int const tw = (r.getWidth() - totalGap) / n;
    int x = r.getX();
    int const h = r.getHeight();

    for (int i = 0; i < n; ++i)
    {
        toggles[static_cast<size_t> (i)]->setBounds (x, r.getY(), tw, h);
        x += tw;
        if (i < n - 1)
        {
            x += kGap;
        }
    }
}

std::unique_ptr<DirektModuleBypassSoloStrip> DirektModuleBypassSoloStrip::tryCreate (
    juce::AudioProcessorValueTreeState& apvts, const juce::String& bypassParamID, const juce::String& soloParamID,
    const juce::String& bypassLabel, const juce::String& soloLabel, const juce::String& bypassTooltip,
    const juce::String& soloTooltip)
{
    if (bypassParamID.isEmpty() && soloParamID.isEmpty())
    {
        return nullptr;
    }
    return std::make_unique<DirektModuleBypassSoloStrip> (apvts, bypassParamID, soloParamID, bypassLabel, soloLabel,
                                                          bypassTooltip, soloTooltip);
}

} // namespace DirektDSP
