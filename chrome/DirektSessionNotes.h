#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include "theme/DirektColours.h"

namespace DirektDSP
{

/** A text panel for session-scoped production notes.
 *  Notes are persisted per session key via juce::ApplicationProperties.
 *  Show this component through DirektPopupPanel or embed it directly. */
class DirektSessionNotes : public juce::Component
{
public:
    /** @param sessionKey  Unique string used as the persistence key (e.g. plugin name + project path). */
    explicit DirektSessionNotes (const juce::String& sessionKey);
    ~DirektSessionNotes() override;

    /** Replace the session key and reload notes for the new key. */
    void setSessionKey (const juce::String& key);

    juce::String getNotes() const;
    void setNotes (const juce::String& text);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void load();
    void save();

    juce::String storageKey;
    juce::TextEditor editor;
    juce::TextButton clearBtn{"Clear"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektSessionNotes)
};

} // namespace DirektDSP
