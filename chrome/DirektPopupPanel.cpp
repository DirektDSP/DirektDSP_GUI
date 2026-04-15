#include "chrome/DirektPopupPanel.h"

namespace DirektDSP
{

DirektPopupPanel::DirektPopupPanel()
{
    setVisible (false);
    setInterceptsMouseClicks (true, true);

    closeBtn.setColour (juce::TextButton::buttonColourId, Colours::bgSection);
    closeBtn.setColour (juce::TextButton::textColourOffId, Colours::textBright);
    closeBtn.onClick = [this] { hide(); };
    addAndMakeVisible (closeBtn);
}

void DirektPopupPanel::show (const juce::String& title, juce::Component* content, int contentW, int contentH)
{
    titleText = title;
    contentComp = content;
    requestedW = contentW;
    requestedH = contentH;

    if (contentComp != nullptr)
    {
        addAndMakeVisible (contentComp);
    }

    setVisible (true);
    toFront (true);
    resized();
    repaint();
}

void DirektPopupPanel::hide()
{
    if (contentComp != nullptr)
    {
        removeChildComponent (contentComp);
    }
    contentComp = nullptr;
    setVisible (false);
}

void DirektPopupPanel::paint (juce::Graphics& g)
{
    // Dimmed background
    g.setColour (Colours::popupDim);
    g.fillRect (getLocalBounds());

    // Panel
    auto panelBounds = getLocalBounds().toFloat();
    auto pw = static_cast<float> (juce::jmin (requestedW, getWidth() - 40));
    auto ph = static_cast<float> (juce::jmin (requestedH + 32, getHeight() - 40));
    auto panel = juce::Rectangle<float> (0.0F, 0.0F, pw, ph).withCentre (panelBounds.getCentre());

    g.setColour (Colours::bgPanel);
    g.fillRoundedRectangle (panel, 8.0F);
    g.setColour (Colours::divider);
    g.drawRoundedRectangle (panel.reduced (0.5F), 8.0F, 1.0F);

    // Title bar
    auto titleBar = panel.removeFromTop (32.0F);
    g.setColour (Colours::bgHeader);
    g.fillRoundedRectangle (titleBar.getX(), titleBar.getY(), titleBar.getWidth(), titleBar.getHeight(), 8.0F);
    // Cover bottom corners of title
    g.fillRect (titleBar.getX(), titleBar.getY() + titleBar.getHeight() - 8.0F, titleBar.getWidth(), 8.0F);

    g.setColour (Colours::textBright);
    g.setFont (juce::Font (juce::FontOptions (13.0F).withStyle ("Bold")));
    g.drawText (titleText, titleBar.reduced (10.0F, 0.0F).toNearestInt(), juce::Justification::centredLeft);
}

void DirektPopupPanel::resized()
{
    auto panelBounds = getLocalBounds().toFloat();
    auto pw = static_cast<float> (juce::jmin (requestedW, getWidth() - 40));
    auto ph = static_cast<float> (juce::jmin (requestedH + 32, getHeight() - 40));
    auto panel = juce::Rectangle<float> (0.0F, 0.0F, pw, ph).withCentre (panelBounds.getCentre());

    auto titleBar = panel.removeFromTop (32.0F);
    closeBtn.setBounds (static_cast<int> (titleBar.getRight()) - 30, static_cast<int> (titleBar.getY()) + 4, 26, 24);

    if (contentComp != nullptr)
    {
        contentComp->setBounds (panel.reduced (8.0F).toNearestInt());
    }
}

void DirektPopupPanel::mouseDown (const juce::MouseEvent& e)
{
    // Dismiss if clicking on dimmed area (outside panel)
    auto panelBounds = getLocalBounds().toFloat();
    auto pw = static_cast<float> (juce::jmin (requestedW, getWidth() - 40));
    auto ph = static_cast<float> (juce::jmin (requestedH + 32, getHeight() - 40));
    auto panel = juce::Rectangle<float> (0.0F, 0.0F, pw, ph).withCentre (panelBounds.getCentre());

    if (!panel.contains (e.position))
    {
        hide();
    }
}

} // namespace DirektDSP
