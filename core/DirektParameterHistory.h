#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

#include <vector>

namespace DirektDSP
{

/**
 * @brief Maintains undo/redo history for APVTS parameter edits.
 *
 * History entries store parameter IDs plus normalized start/end values.
 * Undo/redo applies values via `setValueNotifyingHost()` so host state stays in sync.
 */
class DirektParameterHistory
{
public:
    /**
     * @brief Adds a committed parameter edit to history.
     *
     * @param paramID Parameter identifier in APVTS.
     * @param oldValueNormalized Previous normalized value in range [0, 1].
     * @param newValueNormalized New normalized value in range [0, 1].
     */
    void pushChange (const juce::String& paramID, float oldValueNormalized, float newValueNormalized);

    /**
     * @brief Returns true when at least one undo step is available.
     */
    bool canUndo() const noexcept;

    /**
     * @brief Returns true when at least one redo step is available.
     */
    bool canRedo() const noexcept;

    /**
     * @brief Applies the previous history step.
     *
     * @param apvts Target state containing parameters.
     * @return true when a step was applied.
     */
    bool undo (juce::AudioProcessorValueTreeState& apvts);

    /**
     * @brief Re-applies the next history step.
     *
     * @param apvts Target state containing parameters.
     * @return true when a step was applied.
     */
    bool redo (juce::AudioProcessorValueTreeState& apvts);

private:
    struct ParameterChange
    {
        juce::String paramID;
        float oldValueNormalized = 0.0F;
        float newValueNormalized = 0.0F;
    };

    bool applyValue (juce::AudioProcessorValueTreeState& apvts, const ParameterChange& change, bool applyNewValue);

    std::vector<ParameterChange> history;
    size_t cursor = 0; ///< Number of steps currently applied.
};

} // namespace DirektDSP
