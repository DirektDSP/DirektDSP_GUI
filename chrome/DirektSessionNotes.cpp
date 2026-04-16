#include "chrome/DirektSessionNotes.h"

namespace DirektDSP
{

DirektSessionNotes::DirektSessionNotes (juce::String sessionKey) : storageKey (std::move (sessionKey))
{
    editor.setMultiLine (true);
    editor.setReturnKeyStartsNewLine (true);
    editor.setScrollbarsShown (true);
    editor.setPopupMenuEnabled (true);
    editor.setColour (juce::TextEditor::backgroundColourId, Colours::bgPanel);
    editor.setColour (juce::TextEditor::textColourId, Colours::textBright);
    editor.setColour (juce::TextEditor::outlineColourId, Colours::divider);
    editor.setColour (juce::TextEditor::focusedOutlineColourId, Colours::divider);
    editor.setFont (juce::Font (juce::FontOptions (13.0F)));
    editor.onTextChange = [this] { save(); };
    addAndMakeVisible (editor);

    clearBtn.setColour (juce::TextButton::buttonColourId, Colours::bgSection);
    clearBtn.setColour (juce::TextButton::textColourOffId, Colours::textDim);
    clearBtn.onClick = [this]
    {
        editor.clear();
        save();
    };
    addAndMakeVisible (clearBtn);

    load();
}

DirektSessionNotes::~DirektSessionNotes()
{
    save();
}

void DirektSessionNotes::setSessionKey (const juce::String& key)
{
    save();
    storageKey = key;
    load();
}

juce::String DirektSessionNotes::getNotes() const
{
    return editor.getText();
}

void DirektSessionNotes::setNotes (const juce::String& text)
{
    editor.setText (text, false);
    save();
}

void DirektSessionNotes::paint (juce::Graphics& g)
{
    g.fillAll (Colours::bgPanel);
}

void DirektSessionNotes::resized()
{
    auto bounds = getLocalBounds().reduced (8);
    clearBtn.setBounds (bounds.removeFromBottom (24));
    bounds.removeFromBottom (4);
    editor.setBounds (bounds);
}

void DirektSessionNotes::load()
{
    juce::ApplicationProperties appProps;
    juce::PropertiesFile::Options opts;
    opts.applicationName = "DirektDSP";
    opts.filenameSuffix = ".settings";
    opts.osxLibrarySubFolder = "Application Support";
    appProps.setStorageParameters (opts);

    if (auto* props = appProps.getUserSettings())
    {
        editor.setText (props->getValue (storageKey), false);
    }
}

void DirektSessionNotes::save()
{
    juce::ApplicationProperties appProps;
    juce::PropertiesFile::Options opts;
    opts.applicationName = "DirektDSP";
    opts.filenameSuffix = ".settings";
    opts.osxLibrarySubFolder = "Application Support";
    appProps.setStorageParameters (opts);

    if (auto* props = appProps.getUserSettings())
    {
        props->setValue (storageKey, editor.getText());
        props->saveIfNeeded();
    }
}

} // namespace DirektDSP
