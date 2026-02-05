/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ModalRhodesAudioProcessorEditor::ModalRhodesAudioProcessorEditor(ModalRhodesAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    keyboardComponent(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{

    //cim
    static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::FelipaRegular_ttf, BinaryData::FelipaRegular_ttfSize);

    titleLabel.setText("Modal Rhodes VST", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(typeface).withHeight(35.0f).withStyle(juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, darkTextColour);
    addAndMakeVisible(titleLabel);

    addAndMakeVisible(lvlMeter);
    startTimer(24);

    //debug mode
    addAndMakeVisible(debugButton);
    debugButton.setClickingTogglesState(true);
    debugButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    debugButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::palevioletred);
    debugButton.setButtonText("DEBUG MODE");

    debugButton.onClick = [this] {
        updateVisibility();
        repaint(); //ujrarajzolas
        resized(); //ujratordeli a feluletet
    };

    //teszt elemek
    addAndMakeVisible(runTestsButton);
    runTestsButton.setButtonText("Run Unit Tests");
    runTestsButton.onClick = [this] {
        runner.runAllTests();
        textResults.setText(runner.getFullResults());
        };

    addAndMakeVisible(textResults);
    textResults.setMultiLine(true);
    textResults.setReadOnly(true);
    textResults.setReturnKeyStartsNewLine(true);
    textResults.setFont(juce::Font("Consolas", 14.0f, juce::Font::plain));

    //egyeb labelek
    auto labelSetup = [this](juce::Label& label, std::string title)
        {
            label.setText(title, juce::dontSendNotification);
            label.setFont(juce::Font(15.0f, juce::Font::bold));
            label.setJustificationType(juce::Justification::centred);
            label.setColour(juce::Label::textColourId, textColour);
            addAndMakeVisible(label);
        };

    labelSetup(tremoloLabel, "TREMOLO");
    labelSetup(reverbLabel, "REVERB");
    labelSetup(cabinetLabel, "CABINET SIMULATION");
    labelSetup(delayLabel, "DELAY");
    labelSetup(inLabel, "IN");
    labelSetup(outLabel, "OUT");

    inLabel.setJustificationType(juce::Justification::left);
    outLabel.setJustificationType(juce::Justification::right);

    //keyboard
    keyboardComponent.setAvailableRange(48, 96);
    keyboardComponent.setKeyWidth(30.45);
    addAndMakeVisible(keyboardComponent);

    addAndMakeVisible(cabinetOnButton);
    addAndMakeVisible(cabinetOffButton);

    cabinetOnButton.setClickingTogglesState(true);
    cabinetOffButton.setClickingTogglesState(true);
    cabinetOnButton.setRadioGroupId(101);
    cabinetOffButton.setRadioGroupId(101);


    cabinetOnButton.onClick = [this] {
        if (auto* param = audioProcessor.apvts.getParameter("CABINET_SWITCH"))
            param->setValueNotifyingHost(1.0f);
        };
    cabinetOffButton.onClick = [this] {
        if (auto* param = audioProcessor.apvts.getParameter("CABINET_SWITCH"))
            param->setValueNotifyingHost(0.0f);
        };

    updateCabinetState();

    addAndMakeVisible(tremoloToggleButton);
    tremoloToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "TREM_SWITCH", tremoloToggleButton);

    addAndMakeVisible(reverbToggleButton);
    reverbToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "REVERB_SWITCH", reverbToggleButton);

    addAndMakeVisible(delayToggleButton);
    delayToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "DELAY_SWITCH", delayToggleButton);
    

    struct SliderConfig {
        juce::String id;
        juce::String name;
        ControlGroup group;
        bool isLinear = false;
    };

    std::vector<SliderConfig> config = {

        {"HAMMER_HARDNESS", "Attack", ControlGroup::Main, false},
        {"SUSTAIN_DECAY", "Decay", ControlGroup::Main, false},
        {"DAMPER_RELEASE", "Release", ControlGroup::Main, false},

        {"TREM_DEPTH", "Depth", ControlGroup::Tremolo, false},
        {"TREM_RATE", "Rate", ControlGroup::Tremolo, false},

        {"WET_LEVEL", "Wet Level", ControlGroup::Reverb, false},
        {"DRY_LEVEL", "Dry Level", ControlGroup::Reverb, false},

        {"TIME", "Time", ControlGroup::Delay, false},
        {"FEEDBACK", "Feedback", ControlGroup::Delay, false},
        {"MIX", "Mix", ControlGroup::Delay, false},
        {"TONE", "Tone", ControlGroup::Delay, false},

        {"PREAMP_BASS", "Bass", ControlGroup::Preamp, false},
        {"PREAMP_TREBLE", "Treble", ControlGroup::Preamp, false},
        {"PICKUP_SYMMETRY", "Symmetry", ControlGroup::Preamp, false},//nem preamp de ez igy egyszerubb

        {"PREAMP_DRIVE", "Drive", ControlGroup::Output, true},
        {"OUTPUT_GAIN", "Output", ControlGroup::Output, true},

    };

    for (const auto& cfg : config) {

        addSlider(cfg.id, cfg.name, cfg.group);

        if (cfg.isLinear) {
            auto* s = findSlider(cfg.id);
            if (s) {
                s->slider->setSliderStyle(juce::Slider::LinearVertical);
                s->slider->setLookAndFeel(nullptr); //ehhez meg nincs lnf
            }
        }

    }

    updateVisibility();

    //ablak merete
    setSize(900, 500);
}

ModalRhodesAudioProcessorEditor::~ModalRhodesAudioProcessorEditor()
{

    for (auto& s : sliders) {
        if (s->slider) s->slider->setLookAndFeel(nullptr);
    }
}

void ModalRhodesAudioProcessorEditor::updateVisibility() {

    isDebugOn = debugButton.getToggleState();

    runTestsButton.setVisible(isDebugOn);
    textResults.setVisible(isDebugOn);

    bool normalUI = !isDebugOn;

    keyboardComponent.setVisible(normalUI);

    tremoloLabel.setVisible(normalUI);
    reverbLabel.setVisible(normalUI);
    cabinetLabel.setVisible(normalUI);
    delayLabel.setVisible(normalUI);
    inLabel.setVisible(normalUI);
    outLabel.setVisible(normalUI);

    for (auto& s : sliders) {
        s->setVisible(normalUI);
    }

    juce::Component* otherControls[] = {
        &tremoloToggleButton, &reverbToggleButton, &cabinetOffButton, &cabinetOnButton, &delayToggleButton     
    };

    for (auto* all : otherControls) {
        all->setVisible(normalUI);
    }

}

void ModalRhodesAudioProcessorEditor::updateCabinetState() {
    float value = *audioProcessor.apvts.getRawParameterValue("CABINET_SWITCH");
    bool cabinetOn = value > 0.5f;

    cabinetOnButton.setToggleState(cabinetOn, juce::dontSendNotification);
    cabinetOffButton.setToggleState(cabinetOn, juce::dontSendNotification);
}

//==============================================================================
void ModalRhodesAudioProcessorEditor::paint(juce::Graphics& g)
{
    //hatter
    g.fillAll(backgroundColour);

    if (debugButton.getToggleState()) {
        return;
    }

    auto drawSection = [&](juce::Rectangle<int> bounds, bool accent) {

        auto frame = bounds.toFloat().reduced(5.0f);

        if (!accent) {

            g.setColour(secondaryColour);//hatter
            g.fillRoundedRectangle(frame, 12.0f); //lekerekitett frame

            
        }
        else {
            g.setColour(thirdColour);
            g.fillRoundedRectangle(frame, 12.0f);
        }

        g.setColour(secondaryColour.brighter(0.5f));
        g.drawRoundedRectangle(frame, 12.0f, 1.5f);
        
    };

    drawSection(mainSectionBounds, true);
    drawSection(reverbBounds, false);
    drawSection(tremoloBounds, false);
    drawSection(delayBounds, false);
    drawSection(linearBounds, false);
    drawSection(boostBounds, false);
    drawSection(cabinetBounds, false);

}

//elrendezes
void ModalRhodesAudioProcessorEditor::resized()
{

    if (debugButton.getToggleState()) {

        auto debugArea = getLocalBounds().reduced(20);

        runTestsButton.setBounds(debugArea.removeFromTop(40));
        debugArea.removeFromTop(10);

        textResults.setBounds(debugArea);
    }
    else {
        //normal mod

        //custom knob elhelyezes label miatt
        auto placeKnob = [this](juce::String parameterID, juce::Rectangle<int> bounds, bool main) {
            auto* bundle = findSlider(parameterID);
            if (bundle) {
                juce::Rectangle<int> labelBounds;
                if (!main) labelBounds = bounds.removeFromBottom(15);
                else labelBounds = bounds.removeFromLeft(60);

                bundle->slider->setBounds(bounds);
                bundle->label->setBounds(labelBounds);
            }
           };

        //fo reszek meghatarozasa
        auto totalArea = getLocalBounds().reduced(10);
        auto keyboardAreaHeight = (totalArea.getHeight() / 6) * 2;
        auto controllArea = totalArea.removeFromTop(((totalArea.getHeight() / 6) * 4) - 10);

        //billentyu elhelyezes
        keyboardComponent.setBounds(totalArea.removeFromBottom(keyboardAreaHeight).reduced(5,0));

        //fo arany ertekek
        auto horizontalSectionHeight = controllArea.getHeight() / 3.25;
        auto verticalSectionWidth = controllArea.getWidth() / 7;

        //cim es jobb szekcio
        auto linearSliderArea = controllArea.removeFromRight(verticalSectionWidth);
        auto titleSection = controllArea.removeFromBottom(horizontalSectionHeight);
        mainSectionBounds = titleSection;
        auto titleTextArea = titleSection.removeFromLeft(titleSection.getWidth() / 2.5);
        titleLabel.setBounds(titleTextArea);
        //debugButton.setBounds(titleTextArea.removeFromLeft(100));


        //attack, decay, release knobok
        auto mainKnobAreaWidth = titleSection.getWidth() / 3;
        placeKnob("HAMMER_HARDNESS", titleSection.removeFromLeft(mainKnobAreaWidth).reduced(10), true);
        placeKnob("SUSTAIN_DECAY", titleSection.removeFromLeft(mainKnobAreaWidth).reduced(10), true);
        placeKnob("DAMPER_RELEASE", titleSection.removeFromLeft(mainKnobAreaWidth).reduced(10), true);

        //effekt resz
        auto effectsArea = controllArea.removeFromLeft(verticalSectionWidth * 3);
        auto effectSectionWidth = effectsArea.getWidth() / 4;
        auto labelHeight = horizontalSectionHeight * 0.25;

        //tremolo
        auto tremoloArea = effectsArea.removeFromLeft(effectSectionWidth);
        tremoloLabel.setBounds(tremoloArea.removeFromTop(labelHeight));
        tremoloBounds = tremoloArea;
        tremoloToggleButton.setBounds(tremoloArea.getX() + 5, tremoloArea.getY() + 5, 20, 20);
        placeKnob("TREM_DEPTH", tremoloArea.removeFromTop(horizontalSectionHeight).reduced(10), false);
        placeKnob("TREM_RATE", tremoloArea.removeFromTop(horizontalSectionHeight).reduced(10), false);

        //reverb
        auto reverbArea = effectsArea.removeFromLeft(effectSectionWidth);
        reverbLabel.setBounds(reverbArea.removeFromTop(labelHeight));
        reverbBounds = reverbArea;
        reverbToggleButton.setBounds(reverbArea.getX() + 5, reverbArea.getY() + 5, 20, 20);
        placeKnob("DRY_LEVEL", reverbArea.removeFromTop(horizontalSectionHeight).reduced(10), false);
        placeKnob("WET_LEVEL", reverbArea.removeFromTop(horizontalSectionHeight).reduced(10), false);

        //delay
        auto delayArea = effectsArea.removeFromLeft(effectSectionWidth * 2);
        delayLabel.setBounds(delayArea.removeFromTop(labelHeight));
        delayBounds = delayArea;
        delayToggleButton.setBounds(delayArea.getX() + 5, delayArea.getY() + 5, 20, 20);
        auto delayAreaTop = delayArea.removeFromTop(delayArea.getHeight() / 2);
        placeKnob("TIME", delayAreaTop.removeFromLeft(effectSectionWidth).reduced(10), false);
        placeKnob("FEEDBACK", delayAreaTop.removeFromLeft(effectSectionWidth).reduced(10), false);
        placeKnob("MIX", delayArea.removeFromLeft(effectSectionWidth).reduced(10), false);
        placeKnob("TONE", delayArea.removeFromLeft(effectSectionWidth).reduced(10), false);

        //csuszkak
        auto linearSliderWidth = linearSliderArea.getWidth() / 3;
        auto linearSliderTextArea = linearSliderArea.removeFromTop(labelHeight);
        linearBounds = linearSliderArea;
        auto inputSliderArea = linearSliderArea.removeFromLeft(linearSliderWidth).reduced(0,10);
        auto outputSliderArea = linearSliderArea.removeFromRight(linearSliderWidth).reduced(0, 10);
        auto lvlMeterArea = linearSliderArea.reduced(10, 20);
    

        inLabel.setBounds(linearSliderTextArea.removeFromLeft(50));
        outLabel.setBounds(linearSliderTextArea.removeFromRight(50));

        if(auto* s = findSlider("PREAMP_DRIVE")) s->slider->setBounds(inputSliderArea);
        if (auto* s = findSlider("OUTPUT_GAIN")) s->slider->setBounds(outputSliderArea);

        lvlMeter.setBounds(lvlMeterArea);

        //bass treble symm
        auto boostControllerArea = controllArea.removeFromBottom(horizontalSectionHeight);
        boostBounds = boostControllerArea;
        placeKnob("PREAMP_BASS", boostControllerArea.removeFromLeft(verticalSectionWidth).reduced(10), false);
        placeKnob("PREAMP_TREBLE", boostControllerArea.removeFromLeft(verticalSectionWidth).reduced(10), false);
        placeKnob("PICKUP_SYMMETRY", boostControllerArea.removeFromLeft(verticalSectionWidth).reduced(10), false);

        //maradek hely a cabinet
        controllArea.removeFromTop(labelHeight);
        cabinetBounds = controllArea;
        auto cabinetArea = controllArea.reduced(0,10);
        auto cabinetLabelArea = cabinetArea.removeFromTop(cabinetArea.getHeight()/3);
        cabinetLabel.setBounds(cabinetLabelArea);
        //gombok
        auto buttonArea = cabinetArea.reduced(40, 10);
        auto buttonWidth = buttonArea.getWidth() / 2;
        cabinetOnButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(30,0));
        cabinetOffButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(30,0));

    }
}

//lekerekitett zongora sarkak
void ModalRhodesAudioProcessorEditor::paintOverChildren(juce::Graphics& g) {

    if (isDebugOn || !keyboardComponent.isVisible()) return;

    auto keyboardBounds = keyboardComponent.getBounds().toFloat();
    float radius = 10.0f;

    //teglalap
    juce::Path rectanglePath;
    rectanglePath.addRectangle(keyboardBounds);

    //lekerekitett
    juce::Path roundedPath;
    roundedPath.addRoundedRectangle(keyboardBounds, radius);

    //teglalap - lekerekitett
    juce::Path cornerPath = rectanglePath;
    cornerPath.setUsingNonZeroWinding(false);
    cornerPath.addPath(roundedPath);

    //hatterszin
    g.setColour(juce::Colour(12, 12, 12));
    g.fillPath(cornerPath);

}

void ModalRhodesAudioProcessorEditor::timerCallback() {
    
    float lvl = audioProcessor.currentOutputLevel.load();
    lvlMeter.setLevel(lvl);

}

void ModalRhodesAudioProcessorEditor::addSlider(juce::String parameterID, juce::String name, ControlGroup group) {

    auto newBundle = std::make_unique<SliderStruct>();
    newBundle->parameterID = parameterID;
    newBundle->group = group;

    newBundle->slider = std::make_unique<juce::Slider>();
    newBundle->slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    newBundle->slider->setTextBoxStyle(juce::Slider::NoTextBox, true, 40, 15);

    if (group == ControlGroup::Main) {
        newBundle->slider->setLookAndFeel(&MainKnobsLnF);
    }
    else if( group == ControlGroup::Preamp && (parameterID == "PREAMP_DRIVE" || parameterID == "OUTPUT_GAIN")) {
        //linearis sliderek
    }
    else {
        newBundle->slider->setLookAndFeel(&CustomKnobLnF);
    }

    addAndMakeVisible(*newBundle->slider);

    newBundle->label = std::make_unique<juce::Label>();
    newBundle->label->setText(name, juce::dontSendNotification);
    newBundle->label->setJustificationType(juce::Justification::centred);

    if (group == ControlGroup::Main) {
        newBundle->label->setColour(juce::Label::textColourId, darkTextColour);
    }
    else {
        newBundle->label->setColour(juce::Label::textColourId, textColour);
    }
    addAndMakeVisible(*newBundle->label);

    newBundle->attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, parameterID, *newBundle->slider);

    sliders.push_back(std::move(newBundle));

}

SliderStruct* ModalRhodesAudioProcessorEditor::findSlider(juce::String parameterID) {
    for (auto& s : sliders) {
        if (s->parameterID == parameterID) return s.get();
    }
    return nullptr;
}

