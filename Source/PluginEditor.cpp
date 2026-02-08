/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ModalRhodesAudioProcessorEditor::ModalRhodesAudioProcessorEditor(ModalRhodesAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    keyboardComponent(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    //font
    static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::FelipaRegular_ttf, BinaryData::FelipaRegular_ttfSize);
    static auto regularTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::QuicksandBold_ttf, BinaryData::QuicksandBold_ttfSize);
    static auto regularTypefaceSecond = juce::Typeface::createSystemTypefaceFor(BinaryData::CalSansRegular_ttf, BinaryData::CalSansRegular_ttfSize);
    static auto textboxTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::GeoRegular_ttf, BinaryData::GeoRegular_ttfSize);
    //cim
    
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
    textResults.setFont(juce::Font(textboxTypeface).withHeight(20.0f));

    textResults.setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    textResults.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    textResults.setColour(juce::TextEditor::textColourId, thirdColour);

    //egyeb labelek
    auto labelSetup = [this](juce::Label& label, std::string title)
        {
            label.setText(title, juce::dontSendNotification);
            label.setFont(juce::Font(regularTypefaceSecond).withHeight(20.0f));
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

    auto setupDataLabel = [this](juce::Label& label)
        {
            label.setText("", juce::dontSendNotification);
            label.setFont(juce::Font("Consolas", 14.0f, juce::Font::bold));
            label.setJustificationType(juce::Justification::centred);
            label.setColour(juce::Label::textColourId, textColour);
            //label.setColour(juce::Label::backgroundColourId, secondaryColour);
            addAndMakeVisible(label);
        };

    setupDataLabel(minLabel);
    setupDataLabel(actualLabel);
    setupDataLabel(maxLabel);


    //keyboard
    keyboardComponent.setAvailableRange(48, 96);
    keyboardComponent.setKeyWidth(30.45);
    addAndMakeVisible(keyboardComponent);

    cabinetOnButton.setType(Type::On);
    cabinetOffButton.setType(Type::Off);

    addAndMakeVisible(cabinetOnButton);
    addAndMakeVisible(cabinetOffButton);

    cabinetOnButton.setClickingTogglesState(false);
    cabinetOffButton.setClickingTogglesState(false);
    cabinetOnButton.setRadioGroupId(101);
    cabinetOffButton.setRadioGroupId(101);

    cabinetOnButton.setButtonText("ON");
    cabinetOffButton.setButtonText("OFF");

    cabinetOnButton.onClick = [this] {
        if (auto* param = audioProcessor.apvts.getParameter("CABINET_SWITCH")) {
            if (param->getValue() < 0.5f) param->setValueNotifyingHost(1.0f);
        }
        updateCabinetState();
        };

    cabinetOffButton.onClick = [this] {
        if (auto* param = audioProcessor.apvts.getParameter("CABINET_SWITCH")){
            if (param->getValue() > 0.5f) param->setValueNotifyingHost(0.0f);
        }
        updateCabinetState();
        };

    updateCabinetState();

    addAndMakeVisible(tremoloToggleButton);
    tremoloToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "TREM_SWITCH", tremoloToggleButton);
    tremoloToggleButton.onClick = [this] {updateModuleState(); };

    addAndMakeVisible(reverbToggleButton);
    reverbToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "REVERB_SWITCH", reverbToggleButton);
    reverbToggleButton.onClick = [this] {updateModuleState(); };

    addAndMakeVisible(delayToggleButton);
    delayToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "DELAY_SWITCH", delayToggleButton);
    delayToggleButton.onClick = [this] {updateModuleState(); };

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

    FaderLnFLeft.side = Side::Left;
    FaderLnFRight.side = Side::Right;

    for (const auto& cfg : config) {

        addSlider(cfg.id, cfg.name, cfg.group);

        if (cfg.isLinear) {
            auto* s = findSlider(cfg.id);
            if (s) {
                s->slider->setSliderStyle(juce::Slider::LinearVertical);

                if (cfg.id == "PREAMP_DRIVE") s->slider->setLookAndFeel(&FaderLnFRight);
                else if (cfg.id == "OUTPUT_GAIN") s->slider->setLookAndFeel(&FaderLnFLeft);
            }
        }

    }

    tremoloToggleButton.toFront(true);
    reverbToggleButton.toFront(true);
    delayToggleButton.toFront(true);
    debugButton.toFront(true);

    updateVisibility();
    updateCabinetState();
    updateModuleState();

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
    titleLabel.setVisible(normalUI);

    tremoloLabel.setVisible(normalUI);
    reverbLabel.setVisible(normalUI);
    cabinetLabel.setVisible(normalUI);
    delayLabel.setVisible(normalUI);
    inLabel.setVisible(normalUI);
    outLabel.setVisible(normalUI);

    minLabel.setVisible(normalUI);
    actualLabel.setVisible(normalUI);
    maxLabel.setVisible(normalUI);

    lvlMeter.setVisible(normalUI);

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
    bool cabinetOn = value < 0.5f;

    cabinetOnButton.setToggleState(cabinetOn, juce::dontSendNotification);
    cabinetOffButton.setToggleState(!cabinetOn, juce::dontSendNotification);
}

//==============================================================================
void ModalRhodesAudioProcessorEditor::paint(juce::Graphics& g)
{
    //hatter
    g.fillAll(backgroundColour);

    if (debugButton.getToggleState()) {
        
        if (textResults.isVisible()) {
            auto bounds = textResults.getBounds().toFloat();

            g.setColour(secondaryColour);
            g.fillRoundedRectangle(bounds, 12.0f);

            g.setColour(accentColour);
            g.drawRoundedRectangle(bounds, 12.0f, 1.0f);
        }
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

        g.setColour(secondaryColour.brighter(0.2f));
        g.drawRoundedRectangle(frame, 12.0f, 1.5f);
        
    };

    drawSection(mainSectionBounds, true);

    g.setColour(juce::Colours::black.withAlpha(0.15f));

    auto bounds = mainSectionBounds.toFloat();
    float titleWidth = bounds.getWidth() / 2.7f;
    float knobAreaWidth = bounds.getWidth() - titleWidth;
    float oneKnobWidth = knobAreaWidth / 3.0f - 5.0f;

    float lineTop = bounds.getY() + 15.0f;
    float lineBottom = bounds.getBottom() - 15.0f;

    float x1 = bounds.getX() + titleWidth;
    g.drawLine(x1, lineTop, x1, lineBottom, 1.5f);

    float x2 = x1 + oneKnobWidth;
    g.drawLine(x2, lineTop, x2, lineBottom, 1.5f);

    float x3 = x2 + oneKnobWidth;
    g.drawLine(x3, lineTop, x3, lineBottom, 1.5f);

    drawSection(reverbBounds, false);
    auto reverbArea = reverbBounds.toFloat();
    g.fillRect(reverbArea.getX() + 15, reverbArea.getCentreY(), reverbArea.getWidth() - 30, 1.5f);

    g.setColour(accentColour);
    auto reverbLabelArea = reverbLabelBounds.toFloat();
    g.fillRect(reverbLabelArea.getX() + 20, reverbLabelArea.getY(), reverbLabelArea.getWidth() - 40, 1.5f);

    drawSection(tremoloBounds, false);
    auto tremoloArea = tremoloBounds.toFloat();
    g.fillRect(tremoloArea.getX() + 15, tremoloArea.getCentreY(), tremoloArea.getWidth() - 30, 1.5f);

    auto tremoloLabelArea = tremoloLabelBounds.toFloat();
    g.setColour(accentColour);
    g.fillRect(tremoloLabelArea.getX() + 10, tremoloLabelArea.getY(), tremoloLabelArea.getWidth() - 20, 1.5f);

    drawSection(delayBounds, false);
    auto delayArea = delayBounds.toFloat();

    g.fillRect(delayArea.getX() + 15, delayArea.getCentreY(), delayArea.getWidth()/2 - 30, 1.5f);
    g.fillRect(delayArea.getCentreX() + 15, delayArea.getCentreY(), delayArea.getWidth()/2 - 30, 1.5f);

    g.setColour(accentColour);
    auto delayLabelArea = delayLabelBounds.toFloat();
    g.fillRect(delayLabelArea.getX() + 75, delayLabelArea.getY(), delayLabelArea.getWidth() - 150, 1.5f);

    //g.fillRect(delayArea.getCentreX(), delayArea.getY() + 15, 1.5f, delayArea.getHeight() / 2 - 30);
   // g.fillRect(delayArea.getCentreX(), delayArea.getCentreY() + 15, 1.5f, delayArea.getHeight() / 2 - 30);

    drawSection(linearBounds, false);

    g.setColour(accentColour);
    auto linearLabelArea = linearLabelBounds.toFloat();
    g.fillRect(linearLabelArea.getX() + 25, linearLabelArea.getY(), (linearLabelArea.getWidth() * 5 / 11) - 50, 1.5f);
    g.fillRect(linearLabelArea.getX() + (linearLabelArea.getWidth() * 6 / 11) + 20, linearLabelArea.getY(), (linearLabelArea.getWidth() * 5 / 11) - 40, 1.5f);

    drawSection(boostBounds, false);

    auto boostArea = boostBounds.toFloat();
    //g.setColour(accentColour);
    g.fillRect(boostArea.getX() + (boostArea.getWidth()/3), boostArea.getY() + 15, 1.5f, boostArea.getHeight() - 30);
    g.fillRect(boostArea.getX() + ((boostArea.getWidth()/3) * 2), boostArea.getY() + 15, 1.5f, boostArea.getHeight() - 30);

    drawSection(cabinetBounds, false);

    
    auto numberDataArea = numberDataBounds.toFloat().reduced(5.0f, 2.0f);
    g.setColour(secondaryColour);
    g.fillRect(numberDataArea.getX() + (numberDataArea.getWidth() / 3),numberDataArea.getY(), numberDataArea.getWidth() / 3, numberDataArea.getHeight());
    
    g.setColour(accentColour);
    g.fillRect(numberDataArea.getX() + (numberDataArea.getWidth() / 3), numberDataArea.getY(), 1.5f, numberDataArea.getHeight());
    g.fillRect(numberDataArea.getX() + (numberDataArea.getWidth() * 2/ 3), numberDataArea.getY(), 1.5f, numberDataArea.getHeight());

    

    g.drawRoundedRectangle(numberDataArea, 4.0f, 1.5f);

}

//elrendezes
void ModalRhodesAudioProcessorEditor::resized()
{

    if (debugButton.getToggleState()) {

        auto debugArea = getLocalBounds().reduced(20);

        auto topRowArea = debugArea.removeFromTop(40);
        runTestsButton.setBounds(topRowArea.removeFromLeft(140));
        debugButton.setBounds(topRowArea.getX(), topRowArea.getY() + 20, 20, 20);

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
        auto verticalSectionWidth = controllArea.getWidth() / 7.5;

        //cim es jobb szekcio
        auto linearSliderArea = controllArea.removeFromRight(verticalSectionWidth * 1.5);
        auto titleSection = controllArea.removeFromBottom(horizontalSectionHeight);
        mainSectionBounds = titleSection;
        auto titleTextArea = titleSection.removeFromLeft(titleSection.getWidth() / 2.7);
        titleLabel.setBounds(titleTextArea);
        debugButton.setBounds(titleTextArea.getX() + 5, titleTextArea.getY() + 5, 20, 20);


        //attack, decay, release knobok
        auto mainKnobAreaWidth = titleSection.getWidth() / 3 - 5;
        placeKnob("HAMMER_HARDNESS", titleSection.removeFromLeft(mainKnobAreaWidth).reduced(10), true);
        placeKnob("SUSTAIN_DECAY", titleSection.removeFromLeft(mainKnobAreaWidth).reduced(10), true);
        placeKnob("DAMPER_RELEASE", titleSection.removeFromLeft(mainKnobAreaWidth).reduced(10), true);

        //effekt resz
        auto effectsArea = controllArea.removeFromLeft(verticalSectionWidth * 3.5);
        auto effectSectionWidth = effectsArea.getWidth() / 4;
        auto labelHeight = horizontalSectionHeight * 0.25;

        //tremolo
        auto tremoloArea = effectsArea.removeFromLeft(effectSectionWidth);
        tremoloLabelBounds = tremoloArea.removeFromTop(labelHeight);
        tremoloLabel.setBounds(tremoloLabelBounds);
        tremoloBounds = tremoloArea;
        tremoloToggleButton.setBounds(tremoloArea.getX() + 5, tremoloArea.getY() + 5, 20, 20);
        placeKnob("TREM_DEPTH", tremoloArea.removeFromTop(horizontalSectionHeight).reduced(10), false);
        placeKnob("TREM_RATE", tremoloArea.removeFromTop(horizontalSectionHeight).reduced(10), false);

        //reverb
        auto reverbArea = effectsArea.removeFromLeft(effectSectionWidth);
        reverbLabelBounds = reverbArea.removeFromTop(labelHeight);
        reverbLabel.setBounds(reverbLabelBounds);
        reverbBounds = reverbArea;
        reverbToggleButton.setBounds(reverbArea.getX() + 5, reverbArea.getY() + 5, 20, 20);
        placeKnob("DRY_LEVEL", reverbArea.removeFromTop(horizontalSectionHeight).reduced(10), false);
        placeKnob("WET_LEVEL", reverbArea.removeFromTop(horizontalSectionHeight).reduced(10), false);

        //delay
        auto delayArea = effectsArea.removeFromLeft(effectSectionWidth * 2);
        delayLabelBounds = delayArea.removeFromTop(labelHeight);
        delayLabel.setBounds(delayLabelBounds);
        delayBounds = delayArea;
        delayToggleButton.setBounds(delayArea.getX() + 5, delayArea.getY() + 5, 20, 20);
        auto delayAreaTop = delayArea.removeFromTop(delayArea.getHeight() / 2);
        placeKnob("TIME", delayAreaTop.removeFromLeft(effectSectionWidth).reduced(10), false);
        placeKnob("FEEDBACK", delayAreaTop.removeFromLeft(effectSectionWidth).reduced(10), false);
        placeKnob("MIX", delayArea.removeFromLeft(effectSectionWidth).reduced(10), false);
        placeKnob("TONE", delayArea.removeFromLeft(effectSectionWidth).reduced(10), false);

        //csuszkak
        auto linearSliderWidth = linearSliderArea.getWidth() / 11 * 5;
        linearLabelBounds = linearSliderArea.removeFromTop(labelHeight);
        auto linearSliderTextArea = linearLabelBounds;
        linearBounds = linearSliderArea;
        auto inputSliderArea = linearSliderArea.removeFromLeft(linearSliderWidth).reduced(0,10);
        auto outputSliderArea = linearSliderArea.removeFromRight(linearSliderWidth).reduced(0, 10);
        auto lvlMeterArea = linearSliderArea.reduced(0, 20);
  

        inLabel.setBounds(linearSliderTextArea.removeFromLeft(linearSliderWidth));
        outLabel.setBounds(linearSliderTextArea.removeFromRight(linearSliderWidth));

        if(auto* s = findSlider("PREAMP_DRIVE")) s->slider->setBounds(inputSliderArea);
        if (auto* s = findSlider("OUTPUT_GAIN")) s->slider->setBounds(outputSliderArea);

        lvlMeter.setBounds(lvlMeterArea);

        //bass treble symm
        auto boostControllerAreaHeight = controllArea.getHeight() / 2;
        auto boostControllerArea = controllArea.removeFromBottom(boostControllerAreaHeight);
        boostBounds = boostControllerArea;
        auto boostControllerWidth = boostControllerArea.getWidth() / 3;
        placeKnob("PREAMP_BASS", boostControllerArea.removeFromLeft(boostControllerWidth).reduced(15, 10), false);
        placeKnob("PREAMP_TREBLE", boostControllerArea.removeFromLeft(boostControllerWidth).reduced(15,10), false);
        placeKnob("PICKUP_SYMMETRY", boostControllerArea.removeFromLeft(boostControllerWidth).reduced(15,10), false);

        //data cells
        auto numberDataArea = controllArea.removeFromTop(labelHeight);
        numberDataBounds = numberDataArea;
        auto singleCellWidth = numberDataArea.getWidth() / 3;
        minLabel.setBounds(numberDataArea.removeFromLeft(singleCellWidth).reduced(10,0));
        actualLabel.setBounds(numberDataArea.removeFromLeft(singleCellWidth).reduced(10, 0));
        maxLabel.setBounds(numberDataArea.removeFromLeft(singleCellWidth).reduced(10, 0));

        //maradek hely a cabinet
        cabinetBounds = controllArea;
        auto cabinetArea = controllArea.reduced(0,10);
        auto cabinetLabelArea = cabinetArea.removeFromTop(cabinetArea.getHeight()/3);
        cabinetLabel.setBounds(cabinetLabelArea);
        //gombok
        auto buttonArea = cabinetArea.reduced(70, 0);
        auto buttonWidth = buttonArea.getWidth() / 2;
        cabinetOnButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(0,5));
        cabinetOffButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(0,5));

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
    //lvlmeter frissites
    float lvl = audioProcessor.currentOutputLevel.load();
    lvlMeter.setLevel(lvl);

    //szaminfok frissitese
    bool activeSlider = false;

    for (auto& s : sliders) {
        if (s->slider->isMouseOverOrDragging()) {

            juce::String minimumValue = "MIN: " + juce::String(s->slider->getMinimum(), 1);
            minLabel.setText(minimumValue, juce::dontSendNotification);

            juce::String maximumValue = "MAX: " + juce::String(s->slider->getMaximum(), 1);
            maxLabel.setText(maximumValue, juce::dontSendNotification);

            juce::String actualValue = juce::String(s->slider->getValue(), 1);
            if (s->slider->getTextValueSuffix().isNotEmpty()) {
                actualValue += s->slider->getTextValueSuffix();
            }

            actualLabel.setText(actualValue, juce::dontSendNotification);
            actualLabel.setColour(juce::Label::textColourId, accentColour);

            activeSlider = true;
            break;
        }
    }

    //ha nincs eger semmi felett, akkor toroljuk a texteket
    if (!activeSlider) {
        minLabel.setText("", juce::dontSendNotification);
        actualLabel.setText("", juce::dontSendNotification);
        maxLabel.setText("", juce::dontSendNotification);
        actualLabel.setColour(juce::Label::textColourId, textColour);
    }

}

void ModalRhodesAudioProcessorEditor::addSlider(juce::String parameterID, juce::String name, ControlGroup group) {
    static auto regularTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::QuicksandBold_ttf, BinaryData::QuicksandBold_ttfSize);


    auto newBundle = std::make_unique<SliderStruct>();
    newBundle->parameterID = parameterID;
    newBundle->group = group;

    newBundle->slider = std::make_unique<juce::Slider>();
    newBundle->slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    newBundle->slider->setTextBoxStyle(juce::Slider::NoTextBox, true, 40, 15);

    if (group == ControlGroup::Main) {
        newBundle->slider->setLookAndFeel(&MainKnobsLnF);
    }
    else if( group == ControlGroup::Output && (parameterID == "PREAMP_DRIVE" || parameterID == "OUTPUT_GAIN")) {
        newBundle->slider->setSliderStyle(juce::Slider::LinearVertical);

        if (parameterID == "PREAMP_DRIVE") newBundle->slider->setLookAndFeel(&FaderLnFRight);
        else if (parameterID == "OUTPUT_GAIN") newBundle->slider->setLookAndFeel(&FaderLnFLeft);
    }
    else {
        newBundle->slider->setLookAndFeel(&CustomKnobLnF);
    }

    addAndMakeVisible(*newBundle->slider);

    newBundle->label = std::make_unique<juce::Label>();
    newBundle->label->setText(name, juce::dontSendNotification);

    if(group == ControlGroup::Main) newBundle->label->setFont(juce::Font(regularTypeface).withHeight(22.0f));
    else newBundle->label->setFont(juce::Font(regularTypeface).withHeight(18.0f));

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

void ModalRhodesAudioProcessorEditor::updateModuleState() {

    bool isTremoloOn = *audioProcessor.apvts.getRawParameterValue("TREM_SWITCH");
    bool isReverbOn = *audioProcessor.apvts.getRawParameterValue("REVERB_SWITCH");
    bool isDelayOn = *audioProcessor.apvts.getRawParameterValue("DELAY_SWITCH");

    for (auto& s : sliders) {
        if (s->group == ControlGroup::Tremolo) {
            s->slider->setEnabled(isTremoloOn);
            s->label->setEnabled(isTremoloOn);
        }
        else if (s->group == ControlGroup::Reverb) {
            s->slider->setEnabled(isReverbOn);
            s->label->setEnabled(isReverbOn);
        }
        else if (s->group == ControlGroup::Delay) {
            s->slider->setEnabled(isDelayOn);
            s->label->setEnabled(isDelayOn);
        }
    }

}

