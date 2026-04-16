#include "core/DirektBaseEditor.h"

#include "display/DirektMeter.h"

namespace DirektDSP
{

namespace
{
// Conservative cross-platform filename character whitelist used for autosave file names.
constexpr auto safeFilenameChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";
}

// ============================================================================
// Common init — shared by both constructors
// ============================================================================

void DirektBaseEditor::initCommon (const juce::String& pluginName, juce::Colour accentColour, float ratio,
                                   int /*defaultWidth*/, int minWidth, int maxWidth, bool showHeader, bool showFooter,
                                   bool resizable, bool showTooltips)
{
    aspectRatio = ratio;
    showHeaderFlag = showHeader;
    showFooterFlag = showFooter;

    lookAndFeel.setAccentColour (accentColour);
    setLookAndFeel (&lookAndFeel);
    setWantsKeyboardFocus (true);

    if (showTooltips)
    {
        tooltipWindow = std::make_unique<juce::TooltipWindow> (this, 500);
    }

    if (showHeader)
    {
        addAndMakeVisible (header);
    }
    else
    {
        header.setVisible (false);
    }

    if (showFooter)
    {
        addAndMakeVisible (footer);
    }
    else
    {
        footer.setVisible (false);
    }

    // Popup (always on top, initially hidden)
    addChildComponent (popup);

    // Preset browser (owned, shown via popup)
    presetBrowser = std::make_unique<DirektPresetBrowser> (presetManager, accentColour);
    presetBrowser->onPresetLoaded = [this] { header.updatePresetName(); };
    header.onPresetLabelClicked = [this] { showPopup ("Presets", presetBrowser.get(), 500, 400); };

    // Resizable with fixed aspect ratio
    constrainer.setFixedAspectRatio (static_cast<double> (ratio));
    constrainer.setSizeLimits (minWidth, static_cast<int> (static_cast<float> (minWidth) / ratio), maxWidth,
                               static_cast<int> (static_cast<float> (maxWidth) / ratio));

    if (resizable)
    {
        resizer = std::make_unique<juce::ResizableCornerComponent> (this, &constrainer);
        addAndMakeVisible (resizer.get());
    }

    configureCrashRecovery (pluginName);
}

// ============================================================================
// NEW — config-driven constructor
// ============================================================================

DirektBaseEditor::DirektBaseEditor (juce::AudioProcessor& processor, juce::AudioProcessorValueTreeState& apvts,
                                    Service::PresetManager& presetManager, const PluginConfig& config,
                                    const NodeDescriptor& rootDescriptor)
    : AudioProcessorEditor (processor), apvts (apvts),
      header (config.pluginName, config.accentColour, presetManager, apvts), presetManager (presetManager),
      configDriven (true), buildContext{apvts, lookAndFeel, &parameterHistory, {}}
{
    initCommon (config.pluginName, config.accentColour, config.aspectRatio, config.defaultWidth, config.minWidth,
                config.maxWidth, config.showHeader, config.showFooter, config.resizable, config.showTooltips);
    auto& registry = DirektComponentRegistry::instance();
    rootBuiltNode = registry.build (rootDescriptor, buildContext);

    if (rootBuiltNode.component)
    {
        addAndMakeVisible (rootBuiltNode.component.get());
    }

    // Move owned children
    for (auto& oc : rootBuiltNode.ownedChildren)
    {
        ownedComponents.push_back (std::move (oc));
    }

    // In config-driven mode, call setSize in the base constructor (no subclass needed)
    int const defaultHeight = static_cast<int> (static_cast<float> (config.defaultWidth) / config.aspectRatio);
    setSize (config.defaultWidth, defaultHeight);
}

// ============================================================================
// LEGACY — preserved constructor
// ============================================================================

DirektBaseEditor::DirektBaseEditor (juce::AudioProcessor& processor, juce::AudioProcessorValueTreeState& apvts,
                                    Service::PresetManager& presetManager, const juce::String& pluginName,
                                    juce::Colour accentColour, float aspectRatio, int defaultWidth,
                                    const std::vector<SectionDescriptor>& sectionDescriptors)
    : AudioProcessorEditor (processor), apvts (apvts), header (pluginName, accentColour, presetManager, apvts),
      presetManager (presetManager), buildContext{apvts, lookAndFeel, &parameterHistory, {}}
{
    initCommon (pluginName, accentColour, aspectRatio, defaultWidth, 400, 1600, true, true, true, true);

    // Build sections from descriptors (legacy path)
    builtSections = DirektAutoLayout::buildSections (apvts, sectionDescriptors, &parameterHistory);
    for (auto& bs : builtSections)
    {
        addAndMakeVisible (bs.section.get());
    }

    // Don't call setSize here — derived constructors must call setSize() as their
    // last line so that virtual layoutCustomSections() dispatches correctly and all
    // components added in the derived constructor are already created.
}

// ============================================================================

DirektBaseEditor::~DirektBaseEditor()
{
    stopTimer();

    if (crashRecoveryEnabled)
    {
        crashRecoveryFile.deleteFile();
    }

    setLookAndFeel (nullptr);
}

void DirektBaseEditor::paint (juce::Graphics& g)
{
    g.fillAll (Colours::bgDark);

    auto bounds = getLocalBounds();
    if (showHeaderFlag)
    {
        bounds.removeFromTop (headerHeight);
    }
    if (showFooterFlag)
    {
        bounds.removeFromBottom (footerHeight);
    }

    paintCustomBackground (g, bounds);
}

void DirektBaseEditor::resized()
{
    auto bounds = getLocalBounds();

    if (showHeaderFlag)
    {
        header.setBounds (bounds.removeFromTop (headerHeight));
    }
    if (showFooterFlag)
    {
        footer.setBounds (bounds.removeFromBottom (footerHeight));
    }

    // Resizer in bottom-right corner
    if (resizer)
    {
        resizer->setBounds (getWidth() - 16, getHeight() - 16, 16, 16);
    }

    // Popup covers entire area
    popup.setBounds (getLocalBounds());

    if (configDriven)
    {
        // Config-driven: root component fills the main area
        if (rootBuiltNode.component)
        {
            rootBuiltNode.component->setBounds (bounds);
        }
    }
    else
    {
        // Legacy: let subclass override layout
        layoutCustomSections (bounds);
    }
}

bool DirektBaseEditor::keyPressed (const juce::KeyPress& key)
{
    auto const isCommandDown = key.getModifiers().isCommandDown();
    if (!isCommandDown)
    {
        return AudioProcessorEditor::keyPressed (key);
    }

    auto const keyCode = key.getKeyCode();
    if (keyCode == 'z' || keyCode == 'Z')
    {
        auto const wantsRedo = key.getModifiers().isShiftDown();
        return wantsRedo ? parameterHistory.redo (apvts) : parameterHistory.undo (apvts);
    }

    if (keyCode == 'y' || keyCode == 'Y')
    {
        return parameterHistory.redo (apvts);
    }

    return AudioProcessorEditor::keyPressed (key);
}

void DirektBaseEditor::layoutCustomSections (juce::Rectangle<int> mainArea)
{
    // Default: stack sections vertically with even distribution
    if (builtSections.empty())
    {
        return;
    }

    auto area = mainArea.reduced (8);
    int const sectionCount = static_cast<int> (builtSections.size());
    int const sectionHeight = area.getHeight() / sectionCount;

    for (auto& bs : builtSections)
    {
        bs.section->setBounds (area.removeFromTop (sectionHeight).reduced (0, 2));
    }
}

void DirektBaseEditor::paintCustomBackground (juce::Graphics& /*g*/, juce::Rectangle<int> /*mainArea*/)
{
    // Default: nothing extra
}

DirektSection* DirektBaseEditor::findSection (const juce::String& title)
{
    for (auto& bs : builtSections)
    {
        if (bs.section->getTitle() == title)
        {
            return bs.section.get();
        }
    }
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

// ============================================================================
// New APIs for config-driven mode
// ============================================================================

juce::Component* DirektBaseEditor::findComponentByID (const juce::String& id) const
{
    // Search recursively through all children
    std::function<juce::Component*(juce::Component*)> search;
    search = [&] (juce::Component* parent) -> juce::Component*
    {
        if (parent->getComponentID() == id)
        {
            return parent;
        }
        for (auto* child : parent->getChildren())
        {
            if (auto* found = search (child))
            {
                return found;
            }
        }
        return nullptr;
    };

    return search (const_cast<DirektBaseEditor*> (this));
}

void DirektBaseEditor::bindMeterSource (const juce::String& sourceID, const std::atomic<float>* source)
{
    buildContext.meterSources[sourceID] = source;

    // If the tree is already built, find any meters with matching sourceID and connect them
    std::function<void (juce::Component*)> connectMeters;
    connectMeters = [&] (juce::Component* parent)
    {
        if (parent == nullptr)
        {
            return;
        }

        if (auto* meter = dynamic_cast<DirektMeter*> (parent))
        {
            // Meters don't expose their sourceID, but we can use component ID
            // In practice, meters should be rebuilt or we store the mapping
            meter->setSource (source);
        }
        for (auto* child : parent->getChildren())
        {
            if (child != nullptr)
            {
                connectMeters (child);
            }
        }
    };

    // For meters that need post-build connection, search by component ID
    if (auto* comp = findComponentByID (sourceID))
    {
        if (auto* meter = dynamic_cast<DirektMeter*> (comp))
        {
            meter->setSource (source);
        }
    }
}

void DirektBaseEditor::configureCrashRecovery (const juce::String& pluginName)
{
    if (!juce::JUCEApplicationBase::isStandaloneApp())
    {
        return;
    }

    auto safePluginName = pluginName.retainCharacters (safeFilenameChars);
    if (safePluginName.isEmpty())
    {
        safePluginName = "DirektDSP";
    }

    const auto autosaveDirectory = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                                       .getChildFile ("DirektDSP")
                                       .getChildFile ("crash-recovery");

    if (!autosaveDirectory.exists() && !autosaveDirectory.createDirectory())
    {
        return;
    }

    crashRecoveryFile = autosaveDirectory.getChildFile (safePluginName + "-autosave.xml");
    crashRecoveryEnabled = true;

    if (crashRecoveryFile.existsAsFile())
    {
        auto safeThis = juce::Component::SafePointer<DirektBaseEditor> (this);
        juce::MessageManager::callAsync ([safeThisCopy = safeThis]
        {
            if (safeThisCopy != nullptr)
            {
                safeThisCopy->restoreCrashRecoveryIfNeeded();
            }
        });
    }

    saveCrashRecoverySnapshot();
    startTimer (autosaveIntervalMs);
}

void DirektBaseEditor::saveCrashRecoverySnapshot() const
{
    if (!crashRecoveryEnabled)
    {
        return;
    }

    const auto state = apvts.copyState();
    if (auto xml = state.createXml())
    {
        if (!crashRecoveryFile.replaceWithText (xml->toString()))
        {
            DBG ("DirektDSP: failed to write crash recovery autosave to " + crashRecoveryFile.getFullPathName());
        }
    }
    else
    {
        DBG ("DirektDSP: failed to serialize APVTS state for crash recovery. State type: "
             + state.getType().toString());
    }
}

void DirektBaseEditor::restoreCrashRecoveryIfNeeded()
{
    if (!crashRecoveryFile.existsAsFile())
    {
        return;
    }

    const auto shouldRestore = juce::AlertWindow::showOkCancelBox (
        juce::MessageBoxIconType::QuestionIcon, "Restore previous session?",
        "DirektDSP found autosaved settings from an unexpected shutdown.\n\nRestore these settings?",
        "Restore", "Discard", this);

    if (shouldRestore)
    {
        auto restored = false;

        if (auto xml = juce::parseXML (crashRecoveryFile))
        {
            auto restoredState = juce::ValueTree::fromXml (*xml);
            if (restoredState.isValid())
            {
                apvts.replaceState (restoredState);
                header.updatePresetName();
                restored = true;
            }
        }

        if (!restored)
        {
            crashRecoveryFile.deleteFile();
            juce::AlertWindow::showMessageBoxAsync (
                juce::MessageBoxIconType::WarningIcon, "Autosave could not be restored",
                "The autosave data appears to be invalid and has been discarded.", {}, this);
        }
    }
    else
    {
        crashRecoveryFile.deleteFile();
    }

    saveCrashRecoverySnapshot();
}

void DirektBaseEditor::timerCallback()
{
    saveCrashRecoverySnapshot();
}

} // namespace DirektDSP
