#include <cmath>

#include <juce_gui_basics/juce_gui_basics.h>

#include "StandaloneTestProcessor.h"
#include "theme/DirektColours.h"

namespace
{

/** Dark title bar / text for a non-native shell so the window reads like a DAW plugin frame. */
class PluginHostWindowLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PluginHostWindowLookAndFeel()
    {
        auto scheme = getCurrentColourScheme();
        scheme.setUIColour (juce::LookAndFeel_V4::ColourScheme::widgetBackground, DirektDSP::Colours::bgHeader);
        scheme.setUIColour (juce::LookAndFeel_V4::ColourScheme::defaultText, DirektDSP::Colours::textBright);
        setColourScheme (scheme);
    }
};

class MainWindow : public juce::DocumentWindow, private juce::Timer
{
public:
    explicit MainWindow (StandaloneTestProcessor& proc)
        : DocumentWindow ("DirektDSP — Component Gallery",
                          DirektDSP::Colours::bgDark,
                          juce::DocumentWindow::minimiseButton | juce::DocumentWindow::closeButton),
          processor (proc)
    {
        setUsingNativeTitleBar (false);
        setLookAndFeel (&hostLookAndFeel);
        setBackgroundColour (DirektDSP::Colours::bgDark);

        setContentOwned (processor.createEditor(), true);
        setResizable (true, true);

        auto const minH = juce::roundToInt (static_cast<float> (StandaloneTestLayout::minWidth)
                                            / StandaloneTestLayout::aspectRatio);
        auto const maxH = juce::roundToInt (static_cast<float> (StandaloneTestLayout::maxWidth)
                                            / StandaloneTestLayout::aspectRatio);
        setResizeLimits (StandaloneTestLayout::minWidth, minH, StandaloneTestLayout::maxWidth, maxH);

        centreWithSize (StandaloneTestLayout::defaultWidth, StandaloneTestLayout::defaultHeight());
        startTimerHz (30);
    }

    ~MainWindow() override
    {
        stopTimer();
        setLookAndFeel (nullptr);
    }

    void closeButtonPressed() override { juce::MessageManager::getInstance()->stopDispatchLoop(); }

    void timerCallback() override
    {
        phase += 0.08f;
        processor.meterMono.store (-22.0f + 18.0f * std::sin (phase), std::memory_order_relaxed);
        processor.meterL.store (-28.0f + 12.0f * std::sin (phase * 1.1f), std::memory_order_relaxed);
        processor.meterR.store (-20.0f + 14.0f * std::sin (phase * 0.9f), std::memory_order_relaxed);
        processor.clipLevel.store ((std::fmod (phase, 6.0f) < 0.2f) ? 1.0f : -10.0f, std::memory_order_relaxed);
    }

private:
    StandaloneTestProcessor& processor;
    PluginHostWindowLookAndFeel hostLookAndFeel;
    float phase = 0.0f;
};

} // namespace

int main (int argc, char* argv[])
{
    juce::ScopedJuceInitialiser_GUI juceGui;
    juce::ignoreUnused (argc, argv);

    StandaloneTestProcessor processor;
    MainWindow window (processor);
    window.setVisible (true);

    juce::MessageManager::getInstance()->runDispatchLoop();
    return 0;
}
