#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>

namespace DirektDSP
{

class DirektMidiLearn
{
public:
    /**
     * @brief Adds right-click host parameter menu support for one parameter-bound control.
     *
     * UX behavior:
     * - Right-click on the control opens the host-provided parameter context menu at the mouse position.
     * - Host menu entries (for example MIDI learn, automation, modulation) are provided by the DAW.
     * - If host context is unavailable, right-click does nothing.
     */
    static std::unique_ptr<juce::MouseListener> createHostContextMenuListener (
        juce::Component& target, juce::AudioProcessorParameter* parameter);
};

} // namespace DirektDSP
