#include "DirektBaseEditor.h"

namespace DirektDSP
{

DirektBaseEditor::DirektBaseEditor (juce::AudioProcessor& processor,
                                     juce::AudioProcessorValueTreeState& apvtsRef,
                                     Service::PresetManager& presetManager,
                                     const juce::String& pluginName,
                                     juce::Colour accentColour,
                                     float ratio,
                                     int defaultWidth,
                                     const std::vector<SectionDescriptor>& sectionDescriptors)
    : AudioProcessorEditor (processor),
      apvts (apvtsRef),
      header (pluginName, accentColour, presetManager, apvtsRef),
      aspectRatio (ratio)
{
    lookAndFeel.setAccentColour (accentColour);
    setLookAndFeel (&lookAndFeel);

    addAndMakeVisible (header);
    addAndMakeVisible (footer);

    // Build sections from descriptors
    builtSections = DirektAutoLayout::buildSections (apvts, sectionDescriptors);
    for (auto& bs : builtSections)
        addAndMakeVisible (bs.section.get());

    // Popup (always on top, initially hidden)
    addChildComponent (popup);

    // Resizable with fixed aspect ratio
    int defaultHeight = static_cast<int>(static_cast<float>(defaultWidth) / ratio);
    constrainer.setFixedAspectRatio (static_cast<double>(ratio));
    constrainer.setSizeLimits (400, static_cast<int>(400.0f / ratio),
                               1600, static_cast<int>(1600.0f / ratio));

    resizer = std::make_unique<juce::ResizableCornerComponent> (this, &constrainer);
    addAndMakeVisible (resizer.get());

    setSize (defaultWidth, defaultHeight);
}

DirektBaseEditor::~DirektBaseEditor()
{
    setLookAndFeel (nullptr);
}

void DirektBaseEditor::paint (juce::Graphics& g)
{
    g.fillAll (Colours::bgDark);

    auto bounds = getLocalBounds();
    bounds.removeFromTop (headerHeight);
    bounds.removeFromBottom (footerHeight);

    paintCustomBackground (g, bounds);
}

void DirektBaseEditor::resized()
{
    auto bounds = getLocalBounds();

    header.setBounds (bounds.removeFromTop (headerHeight));
    footer.setBounds (bounds.removeFromBottom (footerHeight));

    // Resizer in bottom-right corner
    resizer->setBounds (getWidth() - 16, getHeight() - 16, 16, 16);

    // Popup covers entire area
    popup.setBounds (getLocalBounds());

    // Let subclass override layout
    layoutCustomSections (bounds);
}

void DirektBaseEditor::layoutCustomSections (juce::Rectangle<int> mainArea)
{
    // Default: stack sections vertically with even distribution
    if (builtSections.empty())
        return;

    auto area = mainArea.reduced (8);
    int sectionCount = static_cast<int>(builtSections.size());
    int sectionHeight = area.getHeight() / sectionCount;

    for (auto& bs : builtSections)
        bs.section->setBounds (area.removeFromTop (sectionHeight).reduced (0, 2));
}

void DirektBaseEditor::paintCustomBackground (juce::Graphics& /*g*/, juce::Rectangle<int> /*mainArea*/)
{
    // Default: nothing extra
}

DirektSection* DirektBaseEditor::findSection (const juce::String& title)
{
    for (auto& bs : builtSections)
        if (bs.section->getTitle() == title)
            return bs.section.get();
    return nullptr;
}

void DirektBaseEditor::showPopup (const juce::String& title, juce::Component* content, int w, int h)
{
    popup.show (title, content, w, h);
}

void DirektBaseEditor::hidePopup()
{
    popup.hide();
}

} // namespace DirektDSP
