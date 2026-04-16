#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "chrome/DirektFooter.h"
#include "chrome/DirektHeader.h"
#include "chrome/DirektPopupPanel.h"
#include "chrome/DirektPresetBrowser.h"
#include "chrome/DirektSessionNotes.h"
#include "config/DirektConfig.h"
#include "config/DirektDescriptors.h"
#include "core/DirektAutoLayout.h"
#include "core/DirektBuildContext.h"
#include "core/DirektComponentRegistry.h"
#include "core/DirektParameterHistory.h"
#include "layout/DirektSection.h"
#include "theme/DirektColours.h"
#include "theme/DirektLookAndFeel.h"

namespace Service
{
class PresetManager;
}

namespace DirektDSP
{

class DirektBaseEditor : public juce::AudioProcessorEditor
{
public:
    // NEW — config-driven, no subclassing needed
    DirektBaseEditor (juce::AudioProcessor& processor, juce::AudioProcessorValueTreeState& apvts,
                      Service::PresetManager& presetManager, const PluginConfig& config,
                      const NodeDescriptor& rootDescriptor);

    // LEGACY — preserved, internally bridges to new system
    DirektBaseEditor (juce::AudioProcessor& processor, juce::AudioProcessorValueTreeState& apvts,
                      Service::PresetManager& presetManager, const juce::String& pluginName, juce::Colour accentColour,
                      float aspectRatio, int defaultWidth, const std::vector<SectionDescriptor>& sectionDescriptors);

    ~DirektBaseEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    /**
     * @brief Handles editor-local undo/redo shortcuts for parameter edits.
     *
     * Supported bindings:
     * - Cmd/Ctrl + Z: undo
     * - Cmd/Ctrl + Shift + Z: redo
     * - Cmd/Ctrl + Y: redo
     */
    bool keyPressed (const juce::KeyPress& key) override;

    // New APIs for config-driven mode
    juce::Component* findComponentByID (const juce::String& id) const;
    void bindMeterSource (const juce::String& sourceID, const std::atomic<float>* source);

    // Session notes API
    void showSessionNotes();
    void setSessionKey (const juce::String& key);

protected:
    // Override points for plugins
    virtual void layoutCustomSections (juce::Rectangle<int> mainArea);
    virtual void paintCustomBackground (juce::Graphics& g, juce::Rectangle<int> mainArea);

    DirektSection* findSection (const juce::String& title);
    void showPopup (const juce::String& title, juce::Component* content, int w, int h);
    void hidePopup();

    juce::AudioProcessorValueTreeState& apvts;
    std::vector<BuiltSection> builtSections; // legacy mode

private:
    void initCommon (const juce::String&, juce::Colour accentColour, float ratio, int, int minWidth, int maxWidth,
                     bool showHeader, bool showFooter, bool resizable, bool showTooltips);

    DirektLookAndFeel lookAndFeel;
    DirektHeader header;
    DirektFooter footer;
    DirektPopupPanel popup;

    std::unique_ptr<juce::TooltipWindow> tooltipWindow;

    juce::ComponentBoundsConstrainer constrainer;
    std::unique_ptr<juce::ResizableCornerComponent> resizer;

    float aspectRatio{};
    Service::PresetManager& presetManager;
    std::unique_ptr<DirektPresetBrowser> presetBrowser;
    std::unique_ptr<DirektSessionNotes> sessionNotes;

    // Config-driven mode members
    bool configDriven = false;
    DirektParameterHistory parameterHistory;
    BuiltNode rootBuiltNode;
    std::vector<std::unique_ptr<juce::Component>> ownedComponents;
    BuildContext buildContext;
    bool showHeaderFlag = true;
    bool showFooterFlag = true;

    static constexpr int headerHeight = 36;
    static constexpr int footerHeight = 22;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektBaseEditor)
};

} // namespace DirektDSP
