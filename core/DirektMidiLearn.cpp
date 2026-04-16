#include "core/DirektMidiLearn.h"

namespace DirektDSP
{

namespace
{
class HostContextMenuMouseListener : public juce::MouseListener
{
public:
    HostContextMenuMouseListener (juce::Component& targetToUse, juce::AudioProcessorParameter* parameterToUse)
        : target (&targetToUse), parameter (parameterToUse)
    {
    }

    ~HostContextMenuMouseListener() override
    {
        if (target != nullptr)
        {
            target->removeMouseListener (this);
        }
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (!e.mods.isPopupMenu() || parameter == nullptr)
        {
            return;
        }

        if (target == nullptr)
        {
            return;
        }

        if (auto* editor = target->findParentComponentOfClass<juce::AudioProcessorEditor>())
        {
            if (auto* context = editor->getHostContext())
            {
                if (auto menu = context->getContextMenuForParameter (parameter))
                {
                    menu->getEquivalentPopupMenu().showMenuAsync (
                        juce::PopupMenu::Options().withTargetComponent (target.getComponent()).withMousePosition());
                }
            }
        }
    }

private:
    juce::Component::SafePointer<juce::Component> target;
    // Non-owning: APVTS parameters outlive parameter-bound controls/listeners.
    juce::AudioProcessorParameter* parameter;
};
} // namespace

std::unique_ptr<juce::MouseListener> DirektMidiLearn::createHostContextMenuListener (
    juce::Component& target, juce::AudioProcessorParameter* parameter)
{
    if (parameter == nullptr)
    {
        return nullptr;
    }

    auto listener = std::make_unique<HostContextMenuMouseListener> (target, parameter);
    target.addMouseListener (listener.get(), false);
    return listener;
}

} // namespace DirektDSP
