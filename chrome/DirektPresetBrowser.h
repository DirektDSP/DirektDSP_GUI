#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>
#include <unordered_set>

#include "theme/DirektColours.h"

namespace Service
{
class PresetManager;
struct PresetMetadata;
} // namespace Service

namespace DirektDSP
{

class DirektPresetBrowser : public juce::Component
{
public:
    /**
     * Creates preset browser bound to preset manager.
     * @param pm Preset service used for list/load/save operations.
     * @param accentColour Accent colour used for highlighted controls.
     */
    DirektPresetBrowser (Service::PresetManager& pm, juce::Colour accentColour);

    void resized() override;
    void paint (juce::Graphics& g) override;
    void visibilityChanged() override;

    std::function<void()> onPresetLoaded;

private:
    struct PresetEntry;

    /** Rebuilds visible filter tags from preset metadata and favorites state. */
    void refreshTags();
    /** Rebuilds preset list using active tag and full-text query filters. */
    void refreshCategories();
    void refreshPresets();
    /** Handles user selecting a tag/category row in sidebar. */
    void onCategorySelected();
    /** Handles user selecting preset row and loads it. */
    void onPresetSelected();
    /** Updates full-text search filter and refreshes results. */
    void onSearchTextChanged();
    /** Toggles favorite state for selected preset row. */
    void onFavoriteToggleRequested (int row);

    /** Opens modal flow for saving current state as new preset. */
    void doSave();
    /** Opens modal flow for deleting selected preset. */
    void doDelete();
    /** Opens modal flow for moving selected preset to other category. */
    void doMove();
    /** Opens modal flow for creating new category/tag bucket. */
    void doNewCategory();
    /** Loads persisted favorites from user settings storage. */
    void loadFavorites();
    /** Saves current favorites set to user settings storage. */
    void saveFavorites() const;
    /** Builds stable key for favorite persistence lookups. */
    static juce::String makeFavoriteKey (const juce::String& name, const juce::String& category);
    /** Returns true if preset currently marked as favorite. */
    bool isFavorite (const PresetEntry& entry) const;

    Service::PresetManager& presetManager;
    juce::Colour accent;

    // --- Category sidebar ---
    class CategoryListModel : public juce::ListBoxModel
    {
    public:
        juce::StringArray categories; // index 0 = "All"
        int selectedRow = 0;
        juce::Colour accent;

        std::function<void()> onSelectionChanged;

        int getNumRows() override { return categories.size(); }
        void paintListBoxItem (int row, juce::Graphics& g, int width, int height, bool isSelected) override;
        void selectedRowsChanged (int lastRowSelected) override
        {
            selectedRow = lastRowSelected;
            if (onSelectionChanged)
                onSelectionChanged();
        }
    };

    CategoryListModel categoryModel;
    juce::ListBox categoryList{"Categories", &categoryModel};

    // --- Preset list ---
    struct PresetEntry
    {
        juce::String name;
        juce::String artist;
        juce::String category;
    };

    class PresetListModel : public juce::ListBoxModel
    {
    public:
        std::vector<PresetEntry> presets;
        int selectedRow = -1;
        juce::Colour accent;
        std::function<bool (const PresetEntry&)> isFavorite;
        std::function<void (int)> onFavoriteToggle;

        std::function<void()> onSelectionChanged;

        int getNumRows() override { return static_cast<int> (presets.size()); }
        void paintListBoxItem (int row, juce::Graphics& g, int width, int height, bool isSelected) override;
        void listBoxItemClicked (int row, const juce::MouseEvent&) override;
    };

    PresetListModel presetModel;
    juce::ListBox presetList{"Presets", &presetModel};
    juce::TextEditor searchEditor;

    // --- Action buttons ---
    juce::TextButton saveBtn{"Save"};
    juce::TextButton deleteBtn{"Delete"};
    juce::TextButton moveBtn{"Move"};
    juce::TextButton newCatBtn{"New Category"};
    std::unordered_set<std::string> favoritePresetKeys;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektPresetBrowser)
};

} // namespace DirektDSP
