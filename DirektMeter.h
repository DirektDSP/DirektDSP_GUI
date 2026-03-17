#pragma once
#include <atomic>
#include <juce_gui_basics/juce_gui_basics.h>
#include "DirektColours.h"

namespace DirektDSP
{

class DirektMeter : public juce::Component, private juce::Timer
{
public:
    enum class Orientation { Vertical, Horizontal };
    enum class Mode { Normal, GainReduction };

    struct Config
    {
        float rangeMinDb = -60.0f;
        float rangeMaxDb = 0.0f;
        Orientation orientation = Orientation::Vertical;
        Mode mode = Mode::Normal;
        juce::Colour colour = Colours::accentRed;
        juce::String label;
        bool peakHold = true;
        int refreshHz = 30;
    };

    explicit DirektMeter (const Config& cfg)
        : config (cfg)
    {
        startTimerHz (config.refreshHz);
    }

    void setSource (const std::atomic<float>* src)  { source = src; }
    void setColour (juce::Colour c)                 { config.colour = c; }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Background
        g.setColour (Colours::knobBg);
        g.fillRoundedRectangle (bounds, 2.0f);

        // Outline
        g.setColour (Colours::divider);
        g.drawRoundedRectangle (bounds.reduced (0.5f), 2.0f, 1.0f);

        float range = config.rangeMaxDb - config.rangeMinDb;
        if (range <= 0.0f) return;

        float normLevel = juce::jlimit (0.0f, 1.0f, (smoothedDb - config.rangeMinDb) / range);
        float normPeak  = juce::jlimit (0.0f, 1.0f, (peakDb - config.rangeMinDb) / range);

        auto inner = bounds.reduced (1.0f);

        if (config.orientation == Orientation::Vertical)
        {
            float barHeight;
            float barY;

            if (config.mode == Mode::GainReduction)
            {
                // Top-down fill
                barHeight = inner.getHeight() * normLevel;
                barY = inner.getY();
            }
            else
            {
                // Bottom-up fill
                barHeight = inner.getHeight() * normLevel;
                barY = inner.getBottom() - barHeight;
            }

            g.setColour (config.colour);
            g.fillRect (inner.getX(), barY, inner.getWidth(), barHeight);

            // Peak hold line
            if (config.peakHold && peakDb > config.rangeMinDb)
            {
                float peakY = (config.mode == Mode::GainReduction)
                    ? inner.getY() + inner.getHeight() * normPeak
                    : inner.getBottom() - inner.getHeight() * normPeak;

                g.setColour (config.colour.brighter (0.4f));
                g.fillRect (inner.getX(), peakY - 1.0f, inner.getWidth(), 2.0f);
            }
        }
        else // Horizontal
        {
            float barWidth;
            float barX;

            if (config.mode == Mode::GainReduction)
            {
                barWidth = inner.getWidth() * normLevel;
                barX = inner.getRight() - barWidth;
            }
            else
            {
                barWidth = inner.getWidth() * normLevel;
                barX = inner.getX();
            }

            g.setColour (config.colour);
            g.fillRect (barX, inner.getY(), barWidth, inner.getHeight());

            if (config.peakHold && peakDb > config.rangeMinDb)
            {
                float peakX = (config.mode == Mode::GainReduction)
                    ? inner.getRight() - inner.getWidth() * normPeak
                    : inner.getX() + inner.getWidth() * normPeak;

                g.setColour (config.colour.brighter (0.4f));
                g.fillRect (peakX - 1.0f, inner.getY(), 2.0f, inner.getHeight());
            }
        }

        // Optional label
        if (config.label.isNotEmpty())
        {
            g.setColour (Colours::textDim);
            g.setFont (juce::Font (juce::FontOptions (10.0f)));
            g.drawText (config.label, getLocalBounds(), juce::Justification::centredBottom);
        }
    }

private:
    void timerCallback() override
    {
        float targetDb = config.rangeMinDb;
        if (source != nullptr)
            targetDb = source->load (std::memory_order_relaxed);

        targetDb = juce::jlimit (config.rangeMinDb, config.rangeMaxDb, targetDb);

        // Exponential smoothing
        constexpr float attackCoeff = 0.6f;
        constexpr float releaseCoeff = 0.15f;

        float coeff = (targetDb > smoothedDb) ? attackCoeff : releaseCoeff;
        smoothedDb += coeff * (targetDb - smoothedDb);

        // Peak hold
        if (targetDb > peakDb)
        {
            peakDb = targetDb;
            peakHoldCounter = config.refreshHz; // ~1 second hold
        }
        else if (peakHoldCounter > 0)
        {
            --peakHoldCounter;
        }
        else
        {
            peakDb += releaseCoeff * (config.rangeMinDb - peakDb);
        }

        repaint();
    }

    Config config;
    const std::atomic<float>* source = nullptr;
    float smoothedDb = -60.0f;
    float peakDb = -60.0f;
    int peakHoldCounter = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirektMeter)
};

} // namespace DirektDSP
