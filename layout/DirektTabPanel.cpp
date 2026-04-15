#include "layout/DirektTabPanel.h"

namespace DirektDSP
{

DirektTabPanel::DirektTabPanel (juce::Colour accentColour) : accent (accentColour) {}

void DirektTabPanel::addTab (const juce::String& label, juce::Component* content)
{
    int idx = static_cast<int> (tabs.size());
    tabs.push_back ({label, content});

    auto btn = std::make_unique<juce::TextButton> (label);
    btn->setColour (juce::TextButton::buttonColourId, Colours::bgSection);
    btn->setColour (juce::TextButton::textColourOffId, Colours::textDim);
    btn->onClick = [this, idx] { setActiveTab (idx); };
    addAndMakeVisible (btn.get());
    tabButtons.push_back (std::move (btn));

    addChildComponent (content);
    updateVisibility();
}

void DirektTabPanel::setActiveTab (int index)
{
    if (index < 0 || index >= static_cast<int> (tabs.size()))
        return;

    activeIndex = index;
    updateVisibility();
    resized();
    repaint();
}

void DirektTabPanel::updateVisibility()
{
    for (int i = 0; i < static_cast<int> (tabs.size()); ++i)
    {
        if (tabs[static_cast<size_t> (i)].content != nullptr)
            tabs[static_cast<size_t> (i)].content->setVisible (i == activeIndex);

        auto& btn = tabButtons[static_cast<size_t> (i)];
        auto textColour = (i == activeIndex) ? accent : Colours::textDim;
        btn->setColour (juce::TextButton::textColourOffId, textColour);
    }
}

void DirektTabPanel::paint (juce::Graphics& g)
{
    // Tab bar background
    auto barArea = getLocalBounds().removeFromTop (tabBarHeight).toFloat();
    g.setColour (Colours::bgHeader);
    g.fillRect (barArea);

    // Active tab indicator
    if (!tabButtons.empty() && activeIndex < static_cast<int> (tabButtons.size()))
    {
        auto& btn = tabButtons[static_cast<size_t> (activeIndex)];
        auto indicatorArea = btn->getBounds().toFloat();
        g.setColour (accent);
        g.fillRect (indicatorArea.getX(), indicatorArea.getBottom() - 2.0f, indicatorArea.getWidth(), 2.0f);
    }
}

void DirektTabPanel::resized()
{
    auto bounds = getLocalBounds();
    auto tabBar = bounds.removeFromTop (tabBarHeight);

    if (!tabButtons.empty())
    {
        int btnW = tabBar.getWidth() / static_cast<int> (tabButtons.size());
        for (auto& btn : tabButtons)
            btn->setBounds (tabBar.removeFromLeft (btnW));
    }

    // Content area
    for (auto& tab : tabs)
        if (tab.content != nullptr && tab.content->isVisible())
            tab.content->setBounds (bounds);
}

} // namespace DirektDSP
