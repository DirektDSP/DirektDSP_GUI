#include "core/DirektParameterHistory.h"

namespace DirektDSP
{

void DirektParameterHistory::pushChange (const juce::String& paramID, float oldValueNormalized,
                                         float newValueNormalized)
{
    if (paramID.isEmpty() || juce::approximatelyEqual (oldValueNormalized, newValueNormalized))
    {
        return;
    }

    if (cursor < history.size())
    {
        history.erase (history.begin() + static_cast<std::ptrdiff_t> (cursor), history.end());
    }

    history.push_back ({paramID, oldValueNormalized, newValueNormalized});
    cursor = history.size();
}

bool DirektParameterHistory::canUndo() const noexcept
{
    return cursor > 0;
}

bool DirektParameterHistory::canRedo() const noexcept
{
    return cursor < history.size();
}

bool DirektParameterHistory::undo (juce::AudioProcessorValueTreeState& apvts)
{
    if (!canUndo())
    {
        return false;
    }

    auto const& change = history[cursor - 1];
    if (!applyValue (apvts, change, false))
    {
        return false;
    }

    --cursor;
    return true;
}

bool DirektParameterHistory::redo (juce::AudioProcessorValueTreeState& apvts)
{
    if (!canRedo())
    {
        return false;
    }

    auto const& change = history[cursor];
    if (!applyValue (apvts, change, true))
    {
        return false;
    }

    ++cursor;
    return true;
}

bool DirektParameterHistory::applyValue (juce::AudioProcessorValueTreeState& apvts, const ParameterChange& change,
                                         bool applyNewValue)
{
    auto* parameter = apvts.getParameter (change.paramID);
    if (parameter == nullptr)
    {
        return false;
    }

    auto const value = applyNewValue ? change.newValueNormalized : change.oldValueNormalized;
    parameter->beginChangeGesture();
    parameter->setValueNotifyingHost (value);
    parameter->endChangeGesture();
    return true;
}

} // namespace DirektDSP
