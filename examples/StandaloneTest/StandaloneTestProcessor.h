#pragma once

#include <atomic>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Service/PresetManager.h"

/** Shared with Main.cpp so the host window matches @ref DirektDSP::PluginConfig in createEditor(). */
namespace StandaloneTestLayout
{
inline constexpr float aspectRatio = 1.65f;
inline constexpr int defaultWidth = 1000;
inline constexpr int minWidth = 640;
inline constexpr int maxWidth = 1920;

inline int defaultHeight() noexcept
{
    return juce::roundToInt (static_cast<float> (defaultWidth) / aspectRatio);
}
} // namespace StandaloneTestLayout

class StandaloneTestProcessor : public juce::AudioProcessor
{
public:
    StandaloneTestProcessor();
    ~StandaloneTestProcessor() override;

    juce::AudioProcessorValueTreeState apvts;

    /** Animated dB values for gallery meters (updated from the main window timer). */
    std::atomic<float> meterMono { -24.0f };
    std::atomic<float> meterL { -30.0f };
    std::atomic<float> meterR { -18.0f };
    /** When >= 0, drives @ref DirektDSP::DirektClipIndicator demo state. */
    std::atomic<float> clipLevel { -6.0f };

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "StandaloneTest"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    class GalleryPresetManager;
    std::unique_ptr<GalleryPresetManager> presetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandaloneTestProcessor)
};
