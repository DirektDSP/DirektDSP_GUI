#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

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
    DirektPresetBrowser (Service::PresetManager& pm, juce::Colour accentColour);

    void resized() override;
    void paint (juce::Graphics& g) override;
    void visibilityChanged() override;

    std::function<void()> onPresetLoaded;

private:
    void refreshCategories();
    void refreshPresets();
    void onCategorySelected();
    void onPresetSelected();

    void doSave();
    void doDelete();
    void doMove();
    void doNewCategory();

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

        std::function<void()> onSelectionChanged;

        int getNumRows() override { return static_cast<int> (presets.size()); }
        void paintListBoxItem (int row, juce::Graphics& g, int width, int height, bool isSelected) override;
        void listBoxItemClicked (int row, const juce::MouseEvent&) override
        {
            selectedRow = row;
            if (onSelectionChanged)
                onSelectionChanged();
        }
    };

    PresetListModel presetModel;
    juce::ListBox presetList{"Presets", &presetModel};

    // --- Action buttons ---
    juce::TextButton saveBtn{"Save"};
    juce::TextButton deleteBtn{"Delete"};
    juce::TextButton moveBtn{"Move"};
    juce::TextButton newCatBtn{"New Category"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektPresetBrowser)
};

} // namespace DirektDSP
