#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

// ============================================================================
// Service::PresetManager – interface stub for DirektDSP_GUI
//
// This header defines the subset of PresetManager that the GUI layer depends
// on.  A concrete implementation lives in each plug-in project; this stub
// keeps the static-analysis build self-contained.
// ============================================================================

namespace Service
{

struct PresetMetadata
{
    /** Human-readable preset display name. */
    juce::String name;
    /** Preset author label shown in browser list. */
    juce::String artist;
    /** Category/group used by preset organization. */
    juce::String category;
    /** Optional free-form tags for text search and filtering. */
    juce::StringArray tags;
};

class PresetManager
{
public:
    virtual ~PresetManager() = default;

    virtual juce::String getCurrentPreset() const = 0;
    virtual juce::String getCurrentCategory() const = 0;

    virtual void loadPreset (const juce::String& name, const juce::String& category) = 0;
    virtual void loadPreviousPreset() = 0;
    virtual void loadNextPreset() = 0;

    virtual void savePreset (const juce::String& name, const juce::String& artist, const juce::String& category) = 0;
    virtual void deletePreset (const juce::String& name, const juce::String& category) = 0;
    virtual void movePresetToCategory (const juce::String& name, const juce::String& fromCategory,
                                       const juce::String& toCategory) = 0;
    virtual void createCategory (const juce::String& name) = 0;

    virtual juce::StringArray getAllCategories() const = 0;
    virtual std::vector<PresetMetadata> getAllPresetMetadata() const = 0;
    virtual std::vector<PresetMetadata> getPresetMetadataInCategory (const juce::String& category) const = 0;
};

} // namespace Service
