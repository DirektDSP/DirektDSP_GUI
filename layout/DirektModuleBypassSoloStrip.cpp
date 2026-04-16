#include "layout/DirektModuleBypassSoloStrip.h"

namespace DirektDSP
{

DirektModuleBypassSoloStrip::DirektModuleBypassSoloStrip (
    juce::AudioProcessorValueTreeState& apvts, const juce::String& bypassParamID, const juce::String& soloParamID,
    const juce::String& bypassLabel, const juce::String& soloLabel, const juce::String& bypassTooltip,
    const juce::String& soloTooltip, const std::atomic<float>* cpuSource, const juce::String& cpuLabelText,
    float cpuWarningThreshold)
    : cpuSource (cpuSource), cpuLabelPrefix (cpuLabelText.isNotEmpty() ? cpuLabelText : juce::String ("CPU")),
      cpuWarningThreshold (juce::jlimit (0.0f, 100.0f, cpuWarningThreshold))
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

    if (this->cpuSource != nullptr)
    {
        cpuLabel.setJustificationType (juce::Justification::centredRight);
        cpuLabel.setFont (juce::Font (juce::FontOptions (11.0F).withStyle ("Bold")));
        addAndMakeVisible (cpuLabel);
        updateCpuLabel();
        startTimerHz (10);
    }
}

int DirektModuleBypassSoloStrip::getIdealWidth() const noexcept
{
    int const n = static_cast<int> (toggles.size());
    int width = n * kToggleMinWidth + (n > 1 ? (n - 1) * kGap : 0);
    if (cpuSource != nullptr)
    {
        width += kCpuMinWidth + (n > 0 ? kGap : 0);
    }
    return width;
}

void DirektModuleBypassSoloStrip::resized()
{
    auto r = getLocalBounds();
    int const n = static_cast<int> (toggles.size());

    if (cpuSource != nullptr)
    {
        int cpuW = juce::jmin (kCpuMinWidth, r.getWidth());
        if (n > 0)
        {
            cpuW = juce::jmin (cpuW, juce::jmax (0, r.getWidth() - (n * kToggleMinWidth + (n - 1) * kGap + kGap)));
        }
        cpuLabel.setBounds (r.removeFromLeft (cpuW));
        if (n > 0 && !r.isEmpty())
        {
            r.removeFromLeft (juce::jmin (kGap, r.getWidth()));
        }
    }

    if (n == 0)
    {
        return;
    }

    int const totalGap = (n - 1) * kGap;
    int const tw = juce::jmax (1, (r.getWidth() - totalGap) / n);
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

void DirektModuleBypassSoloStrip::timerCallback()
{
    updateCpuLabel();
}

void DirektModuleBypassSoloStrip::updateCpuLabel()
{
    if (cpuSource == nullptr)
    {
        return;
    }

    float cpuPercent = cpuSource->load();
    if (cpuPercent <= 1.0F)
    {
        cpuPercent *= 100.0F;
    }
    cpuPercent = juce::jmax (0.0F, cpuPercent);

    auto const textColour = cpuPercent >= cpuWarningThreshold ? juce::Colours::orange : Colours::textDim;
    cpuLabel.setColour (juce::Label::textColourId, textColour);
    cpuLabel.setText (cpuLabelPrefix + " " + juce::String (cpuPercent, 1) + "%", juce::dontSendNotification);
}

std::unique_ptr<DirektModuleBypassSoloStrip> DirektModuleBypassSoloStrip::tryCreate (
    juce::AudioProcessorValueTreeState& apvts, const juce::String& bypassParamID, const juce::String& soloParamID,
    const juce::String& bypassLabel, const juce::String& soloLabel, const juce::String& bypassTooltip,
    const juce::String& soloTooltip, const std::atomic<float>* cpuSource, const juce::String& cpuLabelText,
    float cpuWarningThreshold)
{
    if (bypassParamID.isEmpty() && soloParamID.isEmpty() && cpuSource == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<DirektModuleBypassSoloStrip> (apvts, bypassParamID, soloParamID, bypassLabel, soloLabel,
                                                          bypassTooltip, soloTooltip, cpuSource, cpuLabelText,
                                                          cpuWarningThreshold);
}

} // namespace DirektDSP
