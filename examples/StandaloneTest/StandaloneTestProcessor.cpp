#include "StandaloneTestProcessor.h"

#include <utility>

#include "DirektDSP.h"

class StandaloneTestProcessor::GalleryPresetManager : public Service::PresetManager
{
public:
    juce::String getCurrentPreset() const override { return "—"; }
    juce::String getCurrentCategory() const override { return "Factory"; }

    void loadPreset (const juce::String&, const juce::String&) override {}
    void loadPreviousPreset() override {}
    void loadNextPreset() override {}

    void savePreset (const juce::String&, const juce::String&, const juce::String&) override {}
    void deletePreset (const juce::String&, const juce::String&) override {}
    void movePresetToCategory (const juce::String&, const juce::String&, const juce::String&) override {}
    void createCategory (const juce::String&) override {}

    juce::StringArray getAllCategories() const override { return { "Factory" }; }

    std::vector<Service::PresetMetadata> getAllPresetMetadata() const override { return {}; }

    std::vector<Service::PresetMetadata> getPresetMetadataInCategory (const juce::String&) const override
    {
        return {};
    }
};

namespace
{

using namespace DirektDSP;

NodeDescriptor makeGalleryRootDescriptor()
{
    // Tab: Controls & layout
    VBoxDesc controlsVBox;
    controlsVBox.props.id = "tab_controls";
    controlsVBox.props.size.flex = 1.0f;

    {
        SectionDesc sec;
        sec.props.id = "section_params";
        sec.title = "Parameters";
        sec.columns = 3;

        KnobDesc kGain;
        kGain.props.id = "knob_gain";
        kGain.paramID = "gain";
        kGain.label = "Gain";
        kGain.tooltip = "AudioParameterFloat";
        sec.children.push_back (node (std::move (kGain)));

        ToggleDesc tgl;
        tgl.props.id = "toggle_main";
        tgl.paramID = "toggle_bool";
        tgl.label = "Toggle";
        tgl.tooltip = "AudioParameterBool";
        sec.children.push_back (node (std::move (tgl)));

        ComboBoxDesc combo;
        combo.props.id = "combo_filter";
        combo.paramID = "filter_choice";
        combo.label = "Filter";
        combo.tooltip = "AudioParameterChoice";
        sec.children.push_back (node (std::move (combo)));

        controlsVBox.children.push_back (node (std::move (sec)));
    }

    {
        SectionDesc sec;
        sec.props.id = "section_macro_slider";
        sec.title = "Macro & slider";
        sec.columns = 2;

        MacroDesc macro;
        macro.props.id = "macro_ctrl";
        macro.label = "Macro";
        macro.tooltip = "Maps to two float parameters";
        MacroTargetDesc mtA;
        mtA.paramID = "macro_tgt_a";
        mtA.minNormalized = 0.0f;
        mtA.maxNormalized = 1.0f;
        mtA.curveExponent = 1.0f;
        MacroTargetDesc mtB;
        mtB.paramID = "macro_tgt_b";
        mtB.minNormalized = 0.0f;
        mtB.maxNormalized = 1.0f;
        mtB.curveExponent = 2.0f;
        macro.targets.push_back (mtA);
        macro.targets.push_back (mtB);
        sec.children.push_back (node (std::move (macro)));

        SliderDesc slider;
        slider.props.id = "slider_width";
        slider.paramID = "width";
        slider.label = "Width";
        slider.tooltip = "Horizontal DirektKnob";
        slider.horizontal = true;
        sec.children.push_back (node (std::move (slider)));

        controlsVBox.children.push_back (node (std::move (sec)));
    }

    {
        SectionDesc sec;
        sec.props.id = "section_bypass_solo";
        sec.title = "Module strip";
        sec.columns = 2;
        sec.bypassParamID = "bypass_module";
        sec.soloParamID = "solo_module";

        KnobDesc kDup;
        kDup.paramID = "gain";
        kDup.label = "Gain (dup)";
        sec.children.push_back (node (std::move (kDup)));

        controlsVBox.children.push_back (node (std::move (sec)));
    }

    {
        HBoxDesc row;
        row.props.id = "row_buttons";
        row.props.size.flex = 1.0f;

        ButtonDesc btn;
        btn.props.id = "btn_demo";
        btn.label = "Button";
        btn.tooltip = "juce::TextButton";
        btn.momentary = false;
        row.children.push_back (node (std::move (btn)));

        RadioGroupDesc radio;
        radio.props.id = "radio_demo";
        radio.options = juce::StringArray ("A", "B", "C");
        radio.tooltip = "Radio row";
        row.children.push_back (node (std::move (radio)));

        ABCompareDesc ab;
        ab.props.id = "ab_compare";
        ab.tooltip = "A/B snapshots";
        row.children.push_back (node (std::move (ab)));

        controlsVBox.children.push_back (node (std::move (row)));
    }

    // Tab: Meters
    VBoxDesc metersVBox;
    metersVBox.props.id = "tab_meters";
    metersVBox.props.size.flex = 1.0f;

    {
        HBoxDesc row;
        row.props.id = "meter_row";
        row.props.size.flex = 1.0f;

        MeterDesc mPeak;
        mPeak.props.id = "meter_mono";
        mPeak.sourceID = "meter_mono";
        mPeak.orientation = MeterOrientation::Vertical;
        mPeak.mode = MeterMode::Normal;
        mPeak.label = "Peak";
        row.children.push_back (node (std::move (mPeak)));

        MeterDesc mGr;
        mGr.props.id = "meter_gr";
        mGr.sourceID = "meter_mono";
        mGr.orientation = MeterOrientation::Horizontal;
        mGr.mode = MeterMode::GainReduction;
        mGr.label = "GR";
        row.children.push_back (node (std::move (mGr)));

        MeterDesc mRms;
        mRms.props.id = "meter_rms";
        mRms.sourceID = "meter_mono";
        mRms.orientation = MeterOrientation::Vertical;
        mRms.mode = MeterMode::Rms;
        mRms.label = "RMS";
        row.children.push_back (node (std::move (mRms)));

        metersVBox.children.push_back (node (std::move (row)));
    }

    {
        HBoxDesc row;
        row.props.id = "stereo_clip_row";
        row.props.size.flex = 1.0f;

        StereoMeterDesc stereo;
        stereo.props.id = "stereo_meter";
        stereo.sourceIDLeft = "meter_L";
        stereo.sourceIDRight = "meter_R";
        stereo.orientation = MeterOrientation::Vertical;
        stereo.mode = MeterMode::Lufs;
        stereo.label = "Stereo (LUFS)";
        row.children.push_back (node (std::move (stereo)));

        ClipIndicatorDesc clip;
        clip.props.id = "clip_led";
        clip.sourceID = "clip_src";
        clip.tooltip = "Clip (atomic >= 0 dB)";
        row.children.push_back (node (std::move (clip)));

        metersVBox.children.push_back (node (std::move (row)));
    }

    {
        VBoxDesc labels;
        labels.props.id = "label_col";

        LabelDesc lt;
        lt.text = "Title label";
        lt.fontSize = 18.0f;
        lt.style = "title";
        labels.children.push_back (node (std::move (lt)));

        LabelDesc ls;
        ls.text = "Section label";
        ls.fontSize = 14.0f;
        ls.style = "section";
        labels.children.push_back (node (std::move (ls)));

        LabelDesc lb;
        lb.text = "Body / caption text";
        lb.fontSize = 12.0f;
        lb.style = "body";
        labels.children.push_back (node (std::move (lb)));

        metersVBox.children.push_back (node (std::move (labels)));
    }

    // Tab: Structural
    VBoxDesc structVBox;
    structVBox.props.id = "tab_struct";
    structVBox.props.size.flex = 1.0f;

    {
        DividerDesc div;
        div.props.id = "div_h";
        div.horizontal = true;
        structVBox.children.push_back (node (std::move (div)));
    }

    {
        SpacerDesc sp;
        sp.props.id = "spacer_1";
        sp.props.size.prefH = 16;
        structVBox.children.push_back (node (std::move (sp)));
    }

    {
        XYPadDesc xy;
        xy.props.id = "xypad_placeholder";
        xy.paramX = "gain";
        xy.paramY = "width";
        xy.tooltip = "Placeholder (no widget yet)";
        structVBox.children.push_back (node (std::move (xy)));
    }

    {
        CustomDesc cust;
        cust.props.id = "custom_escape";
        cust.typeKey = "gallery.unregistered";
        structVBox.children.push_back (node (std::move (cust)));
    }

    {
        TabPanelDesc nested;
        nested.props.id = "nested_tabs";

        TabDesc tabA;
        tabA.label = "Nested A";
        LabelDesc nl;
        nl.text = "TabPanel → DirektTabPanel";
        nl.fontSize = 12.0f;
        nl.style = "body";
        tabA.content = node (std::move (nl));
        nested.tabs.push_back (std::move (tabA));

        TabDesc tabB;
        tabB.label = "Nested B";
        KnobDesc nk;
        nk.paramID = "gain";
        nk.label = "Gain";
        tabB.content = node (std::move (nk));
        nested.tabs.push_back (std::move (tabB));

        structVBox.children.push_back (node (std::move (nested)));
    }

    {
        VBoxDesc dragCol;
        dragCol.props.id = "drag_column";
        dragCol.props.draggable = true;
        dragCol.props.size.flex = 1.0f;

        SectionDesc sa;
        sa.title = "Drag reorder A";
        LabelDesc la;
        la.text = "Section in draggable column";
        la.style = "body";
        sa.children.push_back (node (std::move (la)));
        dragCol.children.push_back (node (std::move (sa)));

        SectionDesc sb;
        sb.title = "Drag reorder B";
        LabelDesc lb;
        lb.text = "Grab title bar handle";
        lb.style = "body";
        sb.children.push_back (node (std::move (lb)));
        dragCol.children.push_back (node (std::move (sb)));

        structVBox.children.push_back (node (std::move (dragCol)));
    }

    // Root
    TabPanelDesc mainTabs;
    mainTabs.props.id = "root_tabs";
    mainTabs.props.size.flex = 1.0f;

    {
        TabDesc t0;
        t0.label = "Controls & layout";
        t0.content = node (std::move (controlsVBox));
        mainTabs.tabs.push_back (std::move (t0));
    }
    {
        TabDesc t1;
        t1.label = "Meters & labels";
        t1.content = node (std::move (metersVBox));
        mainTabs.tabs.push_back (std::move (t1));
    }
    {
        TabDesc t2;
        t2.label = "Structural";
        t2.content = node (std::move (structVBox));
        mainTabs.tabs.push_back (std::move (t2));
    }

    VBoxDesc root;
    root.props.id = "gallery_root";
    root.props.size.flex = 1.0f;
    root.children.push_back (node (std::move (mainTabs)));

    return NodeDescriptor (std::move (root));
}

} // namespace

juce::AudioProcessorValueTreeState::ParameterLayout StandaloneTestProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "gain", 1 }, "Gain", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.5f));

    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { "toggle_bool", 1 }, "Toggle", false));

    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { "filter_choice", 1 },
                                                              "Filter", juce::StringArray ("LP", "BP", "HP"), 0));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "width", 1 }, "Width", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.25f));

    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { "bypass_module", 1 }, "Bypass", false));

    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { "solo_module", 1 }, "Solo", false));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "macro_tgt_a", 1 }, "Macro A", juce::NormalisableRange<float> (0.0f, 1.0f, 0.0001f),
        0.5f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "macro_tgt_b", 1 }, "Macro B", juce::NormalisableRange<float> (0.0f, 1.0f, 0.0001f),
        0.5f));

    return layout;
}

StandaloneTestProcessor::StandaloneTestProcessor()
    : juce::AudioProcessor (BusesProperties().withOutput ("Out", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout()),
      presetManager (std::make_unique<GalleryPresetManager>())
{
}

StandaloneTestProcessor::~StandaloneTestProcessor() = default;

void StandaloneTestProcessor::prepareToPlay (double, int) {}

void StandaloneTestProcessor::releaseResources() {}

bool StandaloneTestProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    auto const main = layouts.getMainOutputChannelSet();
    return main == juce::AudioChannelSet::mono() || main == juce::AudioChannelSet::stereo();
}

void StandaloneTestProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ignoreUnused (buffer);
}

juce::AudioProcessorEditor* StandaloneTestProcessor::createEditor()
{
    DirektDSP::PluginConfig cfg;
    cfg.pluginName = "Component Gallery";
    cfg.accentColour = DirektDSP::Colours::accentCyan;
    cfg.aspectRatio = StandaloneTestLayout::aspectRatio;
    cfg.defaultWidth = StandaloneTestLayout::defaultWidth;
    cfg.minWidth = StandaloneTestLayout::minWidth;
    cfg.maxWidth = StandaloneTestLayout::maxWidth;
    cfg.showHeader = true;
    cfg.showFooter = true;
    cfg.resizable = true;
    cfg.showTooltips = true;

    auto* editor = new DirektDSP::DirektBaseEditor (*this, apvts, *presetManager, cfg, makeGalleryRootDescriptor());

    editor->bindMeterSource ("meter_mono", &meterMono);
    editor->bindMeterSource ("meter_L", &meterL);
    editor->bindMeterSource ("meter_R", &meterR);
    editor->bindMeterSource ("clip_src", &clipLevel);

    return editor;
}

void StandaloneTestProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void StandaloneTestProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        if (xml->hasTagName (apvts.state.getType().toString()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
    }
}
