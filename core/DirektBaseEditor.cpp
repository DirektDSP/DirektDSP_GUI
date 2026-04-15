#include "core/DirektBaseEditor.h"

#include "display/DirektMeter.h"

namespace DirektDSP
{

// ============================================================================
// Common init — shared by both constructors
// ============================================================================

void DirektBaseEditor::initCommon (const juce::String& /*pluginName*/, juce::Colour accentColour, float ratio,
                                   int /*defaultWidth*/, int minWidth, int maxWidth, bool showHeader, bool showFooter,
                                   bool resizable, bool showTooltips)
{
    aspectRatio = ratio;
    showHeaderFlag = showHeader;
    showFooterFlag = showFooter;

    lookAndFeel.setAccentColour (accentColour);
    setLookAndFeel (&lookAndFeel);

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
        footer.onCaptureSnapshotA = [this]
        {
            snapshotA = captureCurrentSnapshot();
            updateSnapshotControls();
        };
        footer.onCaptureSnapshotB = [this]
        {
            snapshotB = captureCurrentSnapshot();
            updateSnapshotControls();
            applyMorph (morphPosition);
        };
        footer.onMorphChanged = [this] (float morphValue)
        {
            morphPosition = juce::jlimit (0.0F, 1.0F, morphValue);
            applyMorph (morphPosition);
        };
        updateSnapshotControls();
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
}

// ============================================================================
// NEW — config-driven constructor
// ============================================================================

DirektBaseEditor::DirektBaseEditor (juce::AudioProcessor& processor, juce::AudioProcessorValueTreeState& apvts,
                                    Service::PresetManager& presetManager, const PluginConfig& config,
                                    const NodeDescriptor& rootDescriptor)
    : AudioProcessorEditor (processor), apvts (apvts),
      header (config.pluginName, config.accentColour, presetManager, apvts), presetManager (presetManager),
      configDriven (true), buildContext{apvts, lookAndFeel, {}}
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
      presetManager (presetManager), buildContext{apvts, lookAndFeel, {}}
{
    initCommon (pluginName, accentColour, aspectRatio, defaultWidth, 400, 1600, true, true, true, true);

    // Build sections from descriptors (legacy path)
    builtSections = DirektAutoLayout::buildSections (apvts, sectionDescriptors);
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

    // For meters that need post-build connection, search by component ID
    if (auto* comp = findComponentByID (sourceID))
    {
        if (auto* meter = dynamic_cast<DirektMeter*> (comp))
        {
            meter->setSource (source);
        }
    }
}

DirektBaseEditor::SnapshotState DirektBaseEditor::captureCurrentSnapshot() const
{
    SnapshotState snapshot;
    auto const& parameters = apvts.processor.getParameters();
    for (auto* parameter : parameters)
    {
        if (auto* parameterWithID = dynamic_cast<juce::AudioProcessorParameterWithID*> (parameter))
        {
            snapshot.normalizedValues[parameterWithID->paramID] = parameterWithID->getValue();
        }
    }

    return snapshot;
}

void DirektBaseEditor::applyMorph (float morphValue)
{
    if (!snapshotA.has_value() || !snapshotB.has_value())
    {
        return;
    }

    auto const clampedMorph = juce::jlimit (0.0F, 1.0F, morphValue);
    for (auto const& [paramID, valueA] : snapshotA->normalizedValues)
    {
        auto const iterB = snapshotB->normalizedValues.find (paramID);
        if (iterB == snapshotB->normalizedValues.end())
        {
            continue;
        }

        auto* parameter = apvts.getParameter (paramID);
        if (parameter == nullptr)
        {
            continue;
        }

        auto const targetValue = juce::jmap (clampedMorph, valueA, iterB->second);
        parameter->setValueNotifyingHost (juce::jlimit (0.0F, 1.0F, targetValue));
    }
}

void DirektBaseEditor::updateSnapshotControls()
{
    footer.setSnapshotAvailability (snapshotA.has_value(), snapshotB.has_value());
}

} // namespace DirektDSP
