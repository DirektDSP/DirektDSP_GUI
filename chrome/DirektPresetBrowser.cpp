#include "chrome/DirektPresetBrowser.h"
#include "Service/PresetManager.h"

namespace DirektDSP
{

// ============================================================================
// CategoryListModel
// ============================================================================

void DirektPresetBrowser::CategoryListModel::paintListBoxItem (
    int row, juce::Graphics& g, int width, int height, bool isSelected)
{
    if (row < 0 || row >= categories.size()) return;

    if (isSelected || row == selectedRow)
    {
        g.setColour (accent.withAlpha (0.25f));
        g.fillRect (0, 0, width, height);
    }

    g.setColour (row == selectedRow ? Colours::textBright : Colours::textDim);
    g.setFont (juce::Font (juce::FontOptions (13.0f)));
    g.drawText (categories[row], 8, 0, width - 16, height, juce::Justification::centredLeft);
}

// ============================================================================
// PresetListModel
// ============================================================================

void DirektPresetBrowser::PresetListModel::paintListBoxItem (
    int row, juce::Graphics& g, int width, int height, bool isSelected)
{
    if (row < 0 || row >= static_cast<int> (presets.size())) return;

    if (isSelected || row == selectedRow)
    {
        g.setColour (accent.withAlpha (0.25f));
        g.fillRect (0, 0, width, height);
    }

    auto& entry = presets[static_cast<size_t> (row)];

    g.setColour (row == selectedRow ? Colours::textBright : Colours::textLabel);
    g.setFont (juce::Font (juce::FontOptions (13.0f)));
    g.drawText (entry.name, 8, 0, width / 2, height, juce::Justification::centredLeft);

    g.setColour (Colours::textDim);
    g.setFont (juce::Font (juce::FontOptions (11.0f)));
    g.drawText (entry.artist, width / 2, 0, width / 2 - 8, height, juce::Justification::centredRight);
}

// ============================================================================
// DirektPresetBrowser
// ============================================================================

DirektPresetBrowser::DirektPresetBrowser (Service::PresetManager& pm, juce::Colour accentColour)
    : presetManager (pm), accent (accentColour)
{
    categoryModel.accent = accent;
    presetModel.accent = accent;

    categoryList.setColour (juce::ListBox::backgroundColourId, Colours::bgPanel);
    categoryList.setRowHeight (24);
    addAndMakeVisible (categoryList);

    presetList.setColour (juce::ListBox::backgroundColourId, Colours::bgPanel);
    presetList.setRowHeight (24);
    addAndMakeVisible (presetList);

    categoryModel.onSelectionChanged = [this] { onCategorySelected(); };
    presetModel.onSelectionChanged = [this] { onPresetSelected(); };

    // Button styling
    auto styleButton = [this] (juce::TextButton& btn)
    {
        btn.setColour (juce::TextButton::buttonColourId, Colours::bgSection);
        btn.setColour (juce::TextButton::textColourOffId, Colours::textBright);
        addAndMakeVisible (btn);
    };

    styleButton (saveBtn);
    styleButton (deleteBtn);
    styleButton (moveBtn);
    styleButton (newCatBtn);

    saveBtn.onClick    = [this] { doSave(); };
    deleteBtn.onClick  = [this] { doDelete(); };
    moveBtn.onClick    = [this] { doMove(); };
    newCatBtn.onClick  = [this] { doNewCategory(); };
}

void DirektPresetBrowser::visibilityChanged()
{
    if (isVisible())
    {
        refreshCategories();
        refreshPresets();
    }
}

void DirektPresetBrowser::paint (juce::Graphics& g)
{
    g.fillAll (Colours::bgPanel);
}

void DirektPresetBrowser::resized()
{
    auto bounds = getLocalBounds().reduced (4);
    auto buttonStrip = bounds.removeFromBottom (32);
    bounds.removeFromBottom (4);

    // Category sidebar = 30% width
    auto catArea = bounds.removeFromLeft (bounds.getWidth() * 3 / 10);
    bounds.removeFromLeft (4);

    categoryList.setBounds (catArea);
    presetList.setBounds (bounds);

    // Button strip — equally spaced
    int btnW = buttonStrip.getWidth() / 4;
    saveBtn.setBounds (buttonStrip.removeFromLeft (btnW).reduced (2, 2));
    deleteBtn.setBounds (buttonStrip.removeFromLeft (btnW).reduced (2, 2));
    moveBtn.setBounds (buttonStrip.removeFromLeft (btnW).reduced (2, 2));
    newCatBtn.setBounds (buttonStrip.reduced (2, 2));
}

void DirektPresetBrowser::refreshCategories()
{
    categoryModel.categories.clear();
    categoryModel.categories.add ("All");

    auto cats = presetManager.getAllCategories();
    categoryModel.categories.addArray (cats);

    categoryList.updateContent();
    categoryList.repaint();
}

void DirektPresetBrowser::refreshPresets()
{
    presetModel.presets.clear();

    juce::String selectedCategory;
    int catRow = categoryList.getSelectedRow();
    if (catRow > 0 && catRow < categoryModel.categories.size())
        selectedCategory = categoryModel.categories[catRow];

    if (selectedCategory.isEmpty())
    {
        // "All" — show everything
        auto metadata = presetManager.getAllPresetMetadata();
        for (auto& m : metadata)
            presetModel.presets.push_back ({ m.name, m.artist, m.category });
    }
    else
    {
        auto metadata = presetManager.getPresetMetadataInCategory (selectedCategory);
        for (auto& m : metadata)
            presetModel.presets.push_back ({ m.name, m.artist, m.category });
    }

    presetModel.selectedRow = -1;
    presetList.updateContent();
    presetList.deselectAllRows();
    presetList.repaint();
}

void DirektPresetBrowser::onCategorySelected()
{
    int row = categoryList.getSelectedRow();
    categoryModel.selectedRow = row;
    categoryList.repaint();
    refreshPresets();
}

void DirektPresetBrowser::onPresetSelected()
{
    int row = presetList.getSelectedRow();
    if (row < 0 || row >= static_cast<int> (presetModel.presets.size()))
        return;

    presetModel.selectedRow = row;
    presetList.repaint();

    auto& entry = presetModel.presets[static_cast<size_t> (row)];
    presetManager.loadPreset (entry.name, entry.category);

    if (onPresetLoaded)
        onPresetLoaded();
}

void DirektPresetBrowser::doSave()
{
    auto* aw = new juce::AlertWindow ("Save Preset", "", juce::MessageBoxIconType::NoIcon);
    aw->addTextEditor ("name", "", "Name");
    aw->addTextEditor ("artist", "", "Artist");
    aw->addTextEditor ("category", presetManager.getCurrentCategory(), "Category");
    aw->addButton ("Save", 1);
    aw->addButton ("Cancel", 0);

    aw->enterModalState (true, juce::ModalCallbackFunction::create (
        [this, aw] (int result)
        {
            if (result == 1)
            {
                auto name = aw->getTextEditorContents ("name");
                auto artist = aw->getTextEditorContents ("artist");
                auto category = aw->getTextEditorContents ("category");

                if (name.isNotEmpty())
                {
                    presetManager.savePreset (name, artist, category);
                    refreshCategories();
                    refreshPresets();
                    if (onPresetLoaded)
                        onPresetLoaded();
                }
            }
            delete aw;
        }), true);
}

void DirektPresetBrowser::doDelete()
{
    int row = presetList.getSelectedRow();
    if (row < 0 || row >= static_cast<int> (presetModel.presets.size()))
        return;

    auto& entry = presetModel.presets[static_cast<size_t> (row)];
    auto nameToDelete = entry.name;
    auto catToDelete = entry.category;

    auto* aw = new juce::AlertWindow ("Delete Preset",
        "Delete \"" + nameToDelete + "\"?", juce::MessageBoxIconType::WarningIcon);
    aw->addButton ("Delete", 1);
    aw->addButton ("Cancel", 0);

    aw->enterModalState (true, juce::ModalCallbackFunction::create (
        [this, aw, nameToDelete, catToDelete] (int result)
        {
            if (result == 1)
            {
                presetManager.deletePreset (nameToDelete, catToDelete);
                refreshCategories();
                refreshPresets();
                if (onPresetLoaded)
                    onPresetLoaded();
            }
            delete aw;
        }), true);
}

void DirektPresetBrowser::doMove()
{
    int row = presetList.getSelectedRow();
    if (row < 0 || row >= static_cast<int> (presetModel.presets.size()))
        return;

    auto& entry = presetModel.presets[static_cast<size_t> (row)];
    auto presetName = entry.name;
    auto fromCat = entry.category;

    auto* aw = new juce::AlertWindow ("Move Preset", "Move \"" + presetName + "\" to category:",
                                       juce::MessageBoxIconType::NoIcon);
    aw->addComboBox ("category", presetManager.getAllCategories(), "Category");
    aw->addButton ("Move", 1);
    aw->addButton ("Cancel", 0);

    aw->enterModalState (true, juce::ModalCallbackFunction::create (
        [this, aw, presetName, fromCat] (int result)
        {
            if (result == 1)
            {
                auto toCat = aw->getComboBoxComponent ("category")->getText();
                if (toCat.isNotEmpty() && toCat != fromCat)
                {
                    presetManager.movePresetToCategory (presetName, fromCat, toCat);
                    refreshCategories();
                    refreshPresets();
                }
            }
            delete aw;
        }), true);
}

void DirektPresetBrowser::doNewCategory()
{
    auto* aw = new juce::AlertWindow ("New Category", "", juce::MessageBoxIconType::NoIcon);
    aw->addTextEditor ("name", "", "Category Name");
    aw->addButton ("Create", 1);
    aw->addButton ("Cancel", 0);

    aw->enterModalState (true, juce::ModalCallbackFunction::create (
        [this, aw] (int result)
        {
            if (result == 1)
            {
                auto name = aw->getTextEditorContents ("name");
                if (name.isNotEmpty())
                {
                    presetManager.createCategory (name);
                    refreshCategories();
                }
            }
            delete aw;
        }), true);
}

} // namespace DirektDSP
