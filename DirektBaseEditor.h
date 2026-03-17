#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "DirektColours.h"
#include "DirektLookAndFeel.h"
#include "DirektAutoLayout.h"
#include "DirektHeader.h"
#include "DirektFooter.h"
#include "DirektPopupPanel.h"
#include "DirektPresetBrowser.h"
#include "DirektSection.h"

namespace Service { class PresetManager; }

namespace DirektDSP
{

class DirektBaseEditor : public juce::AudioProcessorEditor
{
public:
    DirektBaseEditor (juce::AudioProcessor& processor,
                      juce::AudioProcessorValueTreeState& apvts,
                      Service::PresetManager& presetManager,
                      const juce::String& pluginName,
                      juce::Colour accentColour,
                      float aspectRatio,
                      int defaultWidth,
                      const std::vector<SectionDescriptor>& sectionDescriptors);

    ~DirektBaseEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

protected:
    // Override points for plugins
    virtual void layoutCustomSections (juce::Rectangle<int> mainArea);
    virtual void paintCustomBackground (juce::Graphics& g, juce::Rectangle<int> mainArea);

    DirektSection* findSection (const juce::String& title);
    void showPopup (const juce::String& title, juce::Component* content, int w, int h);
    void hidePopup();

    juce::AudioProcessorValueTreeState& apvts;
    std::vector<BuiltSection> builtSections;

private:
    DirektLookAndFeel lookAndFeel;
    DirektHeader header;
    DirektFooter footer;
    DirektPopupPanel popup;

    std::unique_ptr<juce::TooltipWindow> tooltipWindow;

    juce::ComponentBoundsConstrainer constrainer;
    std::unique_ptr<juce::ResizableCornerComponent> resizer;

    float aspectRatio;
    Service::PresetManager& presetManager;
    std::unique_ptr<DirektPresetBrowser> presetBrowser;

    static constexpr int headerHeight = 36;
    static constexpr int footerHeight = 22;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektBaseEditor)
};

} // namespace DirektDSP
