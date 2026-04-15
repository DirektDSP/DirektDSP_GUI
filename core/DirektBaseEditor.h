#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <optional>

#include "chrome/DirektFooter.h"
#include "chrome/DirektHeader.h"
#include "chrome/DirektPopupPanel.h"
#include "chrome/DirektPresetBrowser.h"
#include "config/DirektConfig.h"
#include "config/DirektDescriptors.h"
#include "core/DirektAutoLayout.h"
#include "core/DirektBuildContext.h"
#include "core/DirektComponentRegistry.h"
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

    // New APIs for config-driven mode
    juce::Component* findComponentByID (const juce::String& id) const;
    void bindMeterSource (const juce::String& sourceID, const std::atomic<float>* source);

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
    /**
     * @brief Stores normalized values for each automatable parameter.
     */
    struct SnapshotState
    {
        std::map<juce::String, float> normalizedValues;
    };

    /**
     * @brief Captures current APVTS parameter values as normalized snapshot.
     * @return Snapshot of current normalized parameter values.
     */
    SnapshotState captureCurrentSnapshot() const;
    /**
     * @brief Applies interpolated values between two snapshots.
     * @param morphValue Normalized interpolation factor in range [0.0, 1.0].
     */
    void applyMorph (float morphValue);
    /**
     * @brief Updates footer button/slider enabled state from snapshot presence.
     */
    void updateSnapshotControls();

    void initCommon (const juce::String& pluginName, juce::Colour accentColour, float ratio, int defaultWidth,
                     int minWidth, int maxWidth, bool showHeader, bool showFooter, bool resizable, bool showTooltips);

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

    // Config-driven mode members
    bool configDriven = false;
    BuiltNode rootBuiltNode;
    std::vector<std::unique_ptr<juce::Component>> ownedComponents;
    BuildContext buildContext;
    bool showHeaderFlag = true;
    bool showFooterFlag = true;
    std::optional<SnapshotState> snapshotA;
    std::optional<SnapshotState> snapshotB;
    float morphPosition = 0.0F;

    static constexpr int headerHeight = 36;
    static constexpr int footerHeight = 22;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektBaseEditor)
};

} // namespace DirektDSP
