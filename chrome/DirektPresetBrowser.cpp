#include "chrome/DirektPresetBrowser.h"

#include <algorithm>
#include <utility>

#include "Service/PresetManager.h"

namespace DirektDSP
{

// ============================================================================
// CategoryListModel
// ============================================================================

void DirektPresetBrowser::CategoryListModel::paintListBoxItem (int row, juce::Graphics& g, int width, int height,
                                                               bool isSelected)
{
    if (row < 0 || row >= categories.size())
    {
        return;
    }

    if (isSelected || row == selectedRow)
    {
        g.setColour (accent.withAlpha (0.25F));
        g.fillRect (0, 0, width, height);
    }

    g.setColour (row == selectedRow ? Colours::textBright : Colours::textDim);
    g.setFont (juce::Font (juce::FontOptions (13.0F)));
    g.drawText (categories[row], 8, 0, width - 16, height, juce::Justification::centredLeft);
}

// ============================================================================
// PresetListModel
// ============================================================================

void DirektPresetBrowser::PresetListModel::paintListBoxItem (int row, juce::Graphics& g, int width, int height,
                                                             bool isSelected)
{
    if (row < 0 || row >= static_cast<int> (presets.size()))
    {
        return;
    }

    if (isSelected || row == selectedRow)
    {
        g.setColour (accent.withAlpha (0.25F));
        g.fillRect (0, 0, width, height);
    }

    auto& entry = presets[static_cast<size_t> (row)];

    g.setColour (row == selectedRow ? Colours::textBright : Colours::textLabel);
    g.setFont (juce::Font (juce::FontOptions (13.0F)));
    g.drawText (entry.name, 8, 0, width / 2, height, juce::Justification::centredLeft);

    g.setColour (Colours::textDim);
    g.setFont (juce::Font (juce::FontOptions (11.0F)));
    g.drawText (entry.artist, width / 2, 0, width / 2 - 30, height, juce::Justification::centredRight);

    auto const favorite = isFavorite && isFavorite (entry);
    g.setColour (favorite ? Colours::textBright : Colours::textDim);
    g.setFont (juce::Font (juce::FontOptions (14.0F)));
    g.drawText (favorite ? "★" : "☆", width - 24, 0, 16, height, juce::Justification::centred);
}

void DirektPresetBrowser::PresetListModel::listBoxItemClicked (int row, const juce::MouseEvent& event)
{
    if (row < 0 || row >= static_cast<int> (presets.size()))
    {
        return;
    }

    selectedRow = row;
    auto const clickedFavoriteGlyph = event.x >= (event.eventComponent->getWidth() - 28);
    if (clickedFavoriteGlyph && onFavoriteToggle)
    {
        onFavoriteToggle (row);
    }
    else if (onSelectionChanged)
    {
        onSelectionChanged();
    }
}

// ============================================================================
// DirektPresetBrowser
// ============================================================================

DirektPresetBrowser::DirektPresetBrowser (Service::PresetManager& pm, juce::Colour accentColour)
    : presetManager (pm), accent (accentColour)
{
    loadFavorites();

    categoryModel.accent = accent;
    presetModel.accent = accent;
    presetModel.isFavorite = [this] (const PresetEntry& entry) { return isFavorite (entry); };
    presetModel.onFavoriteToggle = [this] (int row) { onFavoriteToggleRequested (row); };

    searchEditor.setTextToShowWhenEmpty ("Search presets, artists, tags...", Colours::textDim);
    searchEditor.setColour (juce::TextEditor::backgroundColourId, Colours::bgSection);
    searchEditor.setColour (juce::TextEditor::textColourId, Colours::textBright);
    searchEditor.setColour (juce::TextEditor::outlineColourId, accent.withAlpha (0.6F));
    searchEditor.onTextChange = [this] { onSearchTextChanged(); };
    addAndMakeVisible (searchEditor);

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

    saveBtn.onClick = [this] { doSave(); };
    deleteBtn.onClick = [this] { doDelete(); };
    moveBtn.onClick = [this] { doMove(); };
    newCatBtn.onClick = [this] { doNewCategory(); };
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
    auto searchArea = bounds.removeFromTop (28);
    bounds.removeFromTop (4);
    auto buttonStrip = bounds.removeFromBottom (32);
    bounds.removeFromBottom (4);

    // Category sidebar = 30% width
    auto catArea = bounds.removeFromLeft (bounds.getWidth() * 3 / 10);
    bounds.removeFromLeft (4);

    searchEditor.setBounds (searchArea);
    categoryList.setBounds (catArea);
    presetList.setBounds (bounds);

    // Button strip — equally spaced
    int const btnW = buttonStrip.getWidth() / 4;
    saveBtn.setBounds (buttonStrip.removeFromLeft (btnW).reduced (2, 2));
    deleteBtn.setBounds (buttonStrip.removeFromLeft (btnW).reduced (2, 2));
    moveBtn.setBounds (buttonStrip.removeFromLeft (btnW).reduced (2, 2));
    newCatBtn.setBounds (buttonStrip.reduced (2, 2));
}

void DirektPresetBrowser::refreshCategories()
{
    refreshTags();
}

void DirektPresetBrowser::refreshTags()
{
    categoryModel.categories.clear();
    categoryModel.categories.add ("All");
    categoryModel.categories.add ("Favorites");

    auto metadata = presetManager.getAllPresetMetadata();
    juce::StringArray tags;

    for (auto const& item : metadata)
    {
        if (item.category.isNotEmpty())
        {
            tags.addIfNotAlreadyThere ("#" + item.category);
        }
        if (item.artist.isNotEmpty())
        {
            tags.addIfNotAlreadyThere ("@" + item.artist);
        }
    }
    tags.sort (true);
    categoryModel.categories.addArray (tags);

    categoryList.updateContent();
    auto selected = juce::jlimit (0, categoryModel.categories.size() - 1, categoryModel.selectedRow);
    categoryModel.selectedRow = selected;
    categoryList.selectRow (selected);
    categoryList.repaint();
}

void DirektPresetBrowser::refreshPresets()
{
    presetModel.presets.clear();

    juce::String selectedCategoryOrTag;
    int const catRow = categoryList.getSelectedRow();
    if (catRow > 0 && catRow < categoryModel.categories.size())
    {
        selectedCategoryOrTag = categoryModel.categories[catRow];
    }

    auto metadata = presetManager.getAllPresetMetadata();
    auto const searchText = searchEditor.getText().toLowerCase().trim();
    auto const useSearch = searchText.isNotEmpty();

    auto tagPasses = [this, &selectedCategoryOrTag] (const Service::PresetMetadata& item)
    {
        if (selectedCategoryOrTag.isEmpty() || selectedCategoryOrTag == "All")
        {
            return true;
        }
        if (selectedCategoryOrTag == "Favorites")
        {
            auto const key = makeFavoriteKey (item.name, item.category).toStdString();
            return favoritePresetKeys.find (key) != favoritePresetKeys.end();
        }
        if (selectedCategoryOrTag.startsWithChar ('#'))
            return item.category == selectedCategoryOrTag.substring (1);
        if (selectedCategoryOrTag.startsWithChar ('@'))
            return item.artist == selectedCategoryOrTag.substring (1);
        return false;
    };

    auto searchPasses = [&searchText, useSearch] (const Service::PresetMetadata& item)
    {
        if (!useSearch)
            return true;
        auto haystack =
            (item.name + " " + item.artist + " " + item.category + " " + item.tags.joinIntoString (" ")).toLowerCase();
        return haystack.contains (searchText);
    };

    for (auto const& item : metadata)
    {
        if (!tagPasses (item) || !searchPasses (item))
            continue;

        presetModel.presets.push_back ({item.name, item.artist, item.category});
    }

    std::sort (presetModel.presets.begin(), presetModel.presets.end(),
               [] (const PresetEntry& lhs, const PresetEntry& rhs)
               { return lhs.name.compareIgnoreCase (rhs.name) < 0; });

    presetModel.selectedRow = -1;
    presetList.updateContent();
    presetList.deselectAllRows();
    presetList.repaint();
}

void DirektPresetBrowser::onCategorySelected()
{
    int const row = categoryList.getSelectedRow();
    categoryModel.selectedRow = row;
    categoryList.repaint();
    refreshPresets();
}

void DirektPresetBrowser::onPresetSelected()
{
    int const row = presetList.getSelectedRow();
    if (row < 0 || row >= static_cast<int> (presetModel.presets.size()))
    {
        return;
    }

    presetModel.selectedRow = row;
    presetList.repaint();

    auto& entry = presetModel.presets[static_cast<size_t> (row)];
    presetManager.loadPreset (entry.name, entry.category);

    if (onPresetLoaded)
    {
        onPresetLoaded();
    }
}

void DirektPresetBrowser::onSearchTextChanged()
{
    refreshPresets();
}

void DirektPresetBrowser::onFavoriteToggleRequested (int row)
{
    if (row < 0 || row >= static_cast<int> (presetModel.presets.size()))
    {
        return;
    }

    auto const& entry = presetModel.presets[static_cast<size_t> (row)];
    auto const key = makeFavoriteKey (entry.name, entry.category).toStdString();
    if (favoritePresetKeys.find (key) != favoritePresetKeys.end())
    {
        favoritePresetKeys.erase (key);
    }
    else
    {
        favoritePresetKeys.insert (key);
    }

    saveFavorites();
    presetList.repaint();
    refreshTags();
    if (categoryModel.selectedRow >= 0 && categoryModel.selectedRow < categoryModel.categories.size() &&
        categoryModel.categories[categoryModel.selectedRow] == "Favorites")
    {
        refreshPresets();
    }
}

void DirektPresetBrowser::doSave()
{
    auto* aw = new juce::AlertWindow ("Save Preset", "", juce::MessageBoxIconType::NoIcon);
    aw->addTextEditor ("name", "", "Name");
    aw->addTextEditor ("artist", "", "Artist");
    aw->addTextEditor ("category", presetManager.getCurrentCategory(), "Category");
    aw->addButton ("Save", 1);
    aw->addButton ("Cancel", 0);

    aw->enterModalState (true,
                         juce::ModalCallbackFunction::create (
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
                                         {
                                             onPresetLoaded();
                                         }
                                     }
                                 }
                                 delete aw;
                             }),
                         true);
}

void DirektPresetBrowser::doDelete()
{
    int const row = presetList.getSelectedRow();
    if (row < 0 || row >= static_cast<int> (presetModel.presets.size()))
    {
        return;
    }

    auto& entry = presetModel.presets[static_cast<size_t> (row)];
    auto nameToDelete = entry.name;
    auto catToDelete = entry.category;

    auto* aw = new juce::AlertWindow ("Delete Preset", "Delete \"" + nameToDelete + "\"?",
                                      juce::MessageBoxIconType::WarningIcon);
    aw->addButton ("Delete", 1);
    aw->addButton ("Cancel", 0);

    aw->enterModalState (true,
                         juce::ModalCallbackFunction::create (
                             [this, aw, nameToDelete, catToDelete] (int result)
                             {
                                 if (result == 1)
                                 {
                                     presetManager.deletePreset (nameToDelete, catToDelete);
                                     favoritePresetKeys.erase (
                                         makeFavoriteKey (nameToDelete, catToDelete).toStdString());
                                     saveFavorites();
                                     refreshCategories();
                                     refreshPresets();
                                     if (onPresetLoaded)
                                     {
                                         onPresetLoaded();
                                     }
                                 }
                                 delete aw;
                             }),
                         true);
}

void DirektPresetBrowser::doMove()
{
    int const row = presetList.getSelectedRow();
    if (row < 0 || row >= static_cast<int> (presetModel.presets.size()))
    {
        return;
    }

    auto& entry = presetModel.presets[static_cast<size_t> (row)];
    auto presetName = entry.name;
    auto fromCat = entry.category;

    auto* aw = new juce::AlertWindow ("Move Preset",
                                      "Move \"" + presetName + "\" to category:", juce::MessageBoxIconType::NoIcon);
    aw->addComboBox ("category", presetManager.getAllCategories(), "Category");
    aw->addButton ("Move", 1);
    aw->addButton ("Cancel", 0);

    aw->enterModalState (true,
                         juce::ModalCallbackFunction::create (
                             [this, aw, presetName, fromCat] (int result)
                             {
                                 if (result == 1)
                                 {
                                     auto toCat = aw->getComboBoxComponent ("category")->getText();
                                     if (toCat.isNotEmpty() && toCat != fromCat)
                                     {
                                         presetManager.movePresetToCategory (presetName, fromCat, toCat);
                                         auto const oldKey = makeFavoriteKey (presetName, fromCat).toStdString();
                                         if (favoritePresetKeys.find (oldKey) != favoritePresetKeys.end())
                                         {
                                             favoritePresetKeys.erase (oldKey);
                                             favoritePresetKeys.insert (
                                                 makeFavoriteKey (presetName, toCat).toStdString());
                                             saveFavorites();
                                         }
                                         refreshCategories();
                                         refreshPresets();
                                     }
                                 }
                                 delete aw;
                             }),
                         true);
}

void DirektPresetBrowser::doNewCategory()
{
    auto* aw = new juce::AlertWindow ("New Category", "", juce::MessageBoxIconType::NoIcon);
    aw->addTextEditor ("name", "", "Category Name");
    aw->addButton ("Create", 1);
    aw->addButton ("Cancel", 0);

    aw->enterModalState (true,
                         juce::ModalCallbackFunction::create (
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
                             }),
                         true);
}

juce::String DirektPresetBrowser::makeFavoriteKey (const juce::String& name, const juce::String& category)
{
    return category.trim() + "::" + name.trim();
}

bool DirektPresetBrowser::isFavorite (const PresetEntry& entry) const
{
    return favoritePresetKeys.find (makeFavoriteKey (entry.name, entry.category).toStdString()) !=
           favoritePresetKeys.end();
}

void DirektPresetBrowser::loadFavorites()
{
    favoritePresetKeys.clear();

    auto favoritesFile = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                             .getChildFile ("DirektDSP")
                             .getChildFile ("preset-browser-favorites.json");

    if (!favoritesFile.existsAsFile())
    {
        return;
    }

    auto parsed = juce::JSON::parse (favoritesFile);
    auto* list = parsed.getArray();
    if (list == nullptr)
    {
        return;
    }

    for (auto const& value : *list)
    {
        auto key = value.toString().trim();
        if (key.isNotEmpty())
        {
            favoritePresetKeys.insert (key.toStdString());
        }
    }
}

void DirektPresetBrowser::saveFavorites() const
{
    juce::Array<juce::var> data;
    for (auto const& key : favoritePresetKeys)
    {
        data.add (juce::String (key));
    }

    auto favoritesFile = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                             .getChildFile ("DirektDSP")
                             .getChildFile ("preset-browser-favorites.json");
    auto const directory = favoritesFile.getParentDirectory();
    if (!directory.exists())
    {
        directory.createDirectory();
    }

    favoritesFile.replaceWithText (juce::JSON::toString (juce::var (data), true));
}

} // namespace DirektDSP
