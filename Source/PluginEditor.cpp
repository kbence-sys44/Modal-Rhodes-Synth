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
    //ablak merete
    setSize (900, 500);

    //cim
    titleLabel.setText("Modal Rhodes VST", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(30.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);


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
            label.setColour(juce::Label::textColourId, juce::Colours::cadetblue);
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

    //segedfuggveny a sok slider miatt
    auto sliderSetup = [this](juce::Slider& slider, juce::Label& label, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment, juce::String paramID, juce::String name) 
        {
            slider.setLookAndFeel(&CustomKnobLnF);

            slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 40, 15);
            addAndMakeVisible(slider);

            label.setText(name, juce::dontSendNotification);
            label.setJustificationType(juce::Justification::centred);
            addAndMakeVisible(label);

            //osszekotes az apvts-el a processzorban
            attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, paramID, slider);
        };

    sliderSetup(hardnessSlider, hardnessLabel, hardnessAttachment, "HAMMER_HARDNESS", "Attack");
    sliderSetup(decaySlider, decayLabel, decayAttachment, "SUSTAIN_DECAY", "Decay");
    sliderSetup(releaseSlider, releaseLabel,releaseAttachment , "DAMPER_RELEASE", "Release");
    sliderSetup(symmetrySlider,symmetryLabel ,symmetryAttachment , "PICKUP_SYMMETRY", "Symmetry");
    sliderSetup(bassSlider,bassLabel,bassAttachment, "PREAMP_BASS", "Bass");
    sliderSetup(trebleSlider,trebleLabel,trebleAttachment, "PREAMP_TREBLE", "Treble");
    
    sliderSetup(tremDepthSlider,tremDepthLabel,tremDepthAttachment, "TREM_DEPTH", "Depth");
    sliderSetup(tremRateSlider,tremRateLabel,tremRateAttachment, "TREM_RATE", "Rate");

    sliderSetup(reverbWetSlider,reverbWetLabel,reverbWetAttachment, "WET_LEVEL", "Wet Level");
    sliderSetup(reverbDrySlider,reverbDryLabel,reverbDryAttachment, "DRY_LEVEL", "Dry Level");

    sliderSetup(driveSlider, driveLabel, driveAttachment, "PREAMP_DRIVE", "Drive");
    sliderSetup(outputSlider, outputLabel, outputAttachment, "OUTPUT_GAIN", "Output");

    sliderSetup(delayTimeSlider, delayTimeLabel, delayTimeAttachment, "TIME", "Time");
    sliderSetup(delayFeedbackSlider, delayFeedbackLabel, delayFeedbackAttachment, "FEEDBACK", "Feedback");
    sliderSetup(delayMixSlider, delayMixLabel, delayMixAttachment, "MIX", "Mix");
    sliderSetup(delayToneSlider, delayToneLabel, delayToneAttachment, "TONE", "Tone");

    driveSlider.setSliderStyle(juce::Slider::LinearVertical);
    outputSlider.setSliderStyle(juce::Slider::LinearVertical);

    addAndMakeVisible(tremoloToggleButton);
    tremoloToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "TREM_SWITH", tremoloToggleButton);

    addAndMakeVisible(reverbToggleButton);
    reverbToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "REVERB_SWITCH", reverbToggleButton);

    addAndMakeVisible(delayToggleButton);
    delayToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "DELAY_SWITCH", delayToggleButton);

    updateVisibility();

}

ModalRhodesAudioProcessorEditor::~ModalRhodesAudioProcessorEditor()
{
    hardnessSlider.setLookAndFeel(nullptr);
    decaySlider.setLookAndFeel(nullptr);
    releaseSlider.setLookAndFeel(nullptr);
    symmetrySlider.setLookAndFeel(nullptr);
    bassSlider.setLookAndFeel(nullptr);
    trebleSlider.setLookAndFeel(nullptr);
    tremDepthSlider.setLookAndFeel(nullptr);
    reverbWetSlider.setLookAndFeel(nullptr);
    reverbDrySlider.setLookAndFeel(nullptr);
    driveSlider.setLookAndFeel(nullptr);
    outputSlider.setLookAndFeel(nullptr);
    delayFeedbackSlider.setLookAndFeel(nullptr);
    delayTimeSlider.setLookAndFeel(nullptr);
    delayMixSlider.setLookAndFeel(nullptr);
    delayToneSlider.setLookAndFeel(nullptr);
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

    juce::Component* standardControls[] = {
        &hardnessSlider, &hardnessLabel, &decaySlider, &decayLabel,
        &releaseSlider, &releaseLabel, &symmetrySlider, &symmetryLabel,
        &bassSlider, &bassLabel, &trebleSlider, &trebleLabel,
        &tremDepthSlider, &tremDepthLabel, &tremRateSlider, &tremRateLabel,
        &reverbDrySlider, &reverbDryLabel, &reverbWetSlider, &reverbWetLabel,
        &driveSlider, &driveLabel, &outputSlider, &outputLabel,
        &tremoloToggleButton, &reverbToggleButton, &cabinetOffButton, &cabinetOnButton, &delayToggleButton,
        &delayTimeSlider, &delayFeedbackSlider, &delayMixSlider, &delayToneSlider        
    };

    for (auto* all : standardControls) {
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
    g.fillAll(juce::Colour(0xff2b2b2b));

    //elvalaszto vonal
    //g.setColour(juce::Colours::white.withAlpha(0.2f));
    //g.drawLine(20, 50, getWidth() - 20, 50, 1.0f);

    if (debugButton.getToggleState()) {
        return;
    }

    auto drawSection = [&](juce::Rectangle<int> bounds) {

        auto frame = bounds.toFloat().reduced(5.0f);

        g.setColour(juce::Colour(0xff333333)); //hatter
        g.fillRoundedRectangle(frame, 10.0f); //lekerekitett frame

        g.setColour(juce::Colours::white.withAlpha(0.15f));
        g.drawRoundedRectangle(frame, 10.0f, 1.5f);
    };

    drawSection(mainSectionBounds);
    drawSection(reverbBounds);
    drawSection(tremoloBounds);
    drawSection(delayBounds);
    drawSection(linearBounds);
    drawSection(boostBounds);
    drawSection(cabinetBounds);

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
        auto placeKnob = [](juce::Slider& slider, juce::Label& label, juce::Rectangle<int> bounds) {
            auto labelBounds = bounds.removeFromBottom(15);
            slider.setBounds(bounds);
            label.setBounds(labelBounds);
           };

        //fo reszek meghatarozasa
        auto totalArea = getLocalBounds().reduced(10);
        auto keyboardAreaHeight = (totalArea.getHeight() / 6) * 2;
        auto controllArea = totalArea.removeFromTop(((totalArea.getHeight() / 6) * 4) - 10);

        //billentyu elhelyezes
        keyboardComponent.setBounds(totalArea.removeFromBottom(keyboardAreaHeight));

        //fo arany ertekek
        auto horizontalSectionHeight = controllArea.getHeight() / 3.25;
        auto verticalSectionWidth = controllArea.getWidth() / 7;

        //cim es jobb szekcio
        auto linearSliderArea = controllArea.removeFromRight(verticalSectionWidth);
        auto titleSection = controllArea.removeFromBottom(horizontalSectionHeight);
        mainSectionBounds = titleSection;
        auto titleTextArea = titleSection.removeFromLeft(titleSection.getWidth() / 2);
        titleLabel.setBounds(titleTextArea);
        //debugButton.setBounds(titleTextArea.removeFromLeft(100));


        //attack, decay, release knobok
        placeKnob(hardnessSlider, hardnessLabel, titleSection.removeFromLeft(verticalSectionWidth).reduced(10));
        placeKnob(decaySlider, decayLabel, titleSection.removeFromLeft(verticalSectionWidth).reduced(10));
        placeKnob(releaseSlider, releaseLabel, titleSection.removeFromLeft(verticalSectionWidth).reduced(10));

        //effekt resz
        auto effectsArea = controllArea.removeFromLeft(verticalSectionWidth * 3);
        auto effectSectionWidth = effectsArea.getWidth() / 4;
        auto labelHeight = horizontalSectionHeight * 0.25;

        //tremolo
        auto tremoloArea = effectsArea.removeFromLeft(effectSectionWidth);
        tremoloLabel.setBounds(tremoloArea.removeFromTop(labelHeight));
        tremoloBounds = tremoloArea;
        tremoloToggleButton.setBounds(tremoloArea.getX() + 5, tremoloArea.getY() + 5, 20, 20);
        placeKnob(tremDepthSlider, tremDepthLabel, tremoloArea.removeFromTop(horizontalSectionHeight).reduced(10));
        placeKnob(tremRateSlider, tremRateLabel, tremoloArea.removeFromTop(horizontalSectionHeight).reduced(10));

        //reverb
        auto reverbArea = effectsArea.removeFromLeft(effectSectionWidth);
        reverbLabel.setBounds(reverbArea.removeFromTop(labelHeight));
        reverbBounds = reverbArea;
        reverbToggleButton.setBounds(reverbArea.getX() + 5, reverbArea.getY() + 5, 20, 20);
        placeKnob(reverbDrySlider, reverbDryLabel, reverbArea.removeFromTop(horizontalSectionHeight).reduced(10));
        placeKnob(reverbWetSlider, reverbWetLabel, reverbArea.removeFromTop(horizontalSectionHeight).reduced(10));

        //delay
        auto delayArea = effectsArea.removeFromLeft(effectSectionWidth * 2);
        delayLabel.setBounds(delayArea.removeFromTop(labelHeight));
        delayBounds = delayArea;
        delayToggleButton.setBounds(delayArea.getX() + 5, delayArea.getY() + 5, 20, 20);
        auto delayAreaTop = delayArea.removeFromTop(delayArea.getHeight() / 2);
        placeKnob(delayTimeSlider, delayTimeLabel, delayAreaTop.removeFromLeft(effectSectionWidth).reduced(10));
        placeKnob(delayFeedbackSlider, delayFeedbackLabel, delayAreaTop.removeFromLeft(effectSectionWidth).reduced(10));
        placeKnob(delayMixSlider, delayMixLabel, delayArea.removeFromLeft(effectSectionWidth).reduced(10));
        placeKnob(delayToneSlider, delayToneLabel, delayArea.removeFromLeft(effectSectionWidth).reduced(10));

        //csuszkak
        auto linearSliderWidth = linearSliderArea.getWidth() / 3;
        auto linearSliderTextArea = linearSliderArea.removeFromTop(labelHeight);
        linearBounds = linearSliderArea;
        auto inputSliderArea = linearSliderArea.removeFromLeft(linearSliderWidth).reduced(0,10);
        auto outputSliderArea = linearSliderArea.removeFromRight(linearSliderWidth).reduced(0, 10);

        inLabel.setBounds(linearSliderTextArea.removeFromLeft(50));
        outLabel.setBounds(linearSliderTextArea.removeFromRight(50));
        driveSlider.setBounds(inputSliderArea);
        outputSlider.setBounds(outputSliderArea);

        //bass treble symm
        auto boostControllerArea = controllArea.removeFromBottom(horizontalSectionHeight);
        boostBounds = boostControllerArea;
        placeKnob(bassSlider, bassLabel, boostControllerArea.removeFromLeft(verticalSectionWidth).reduced(10));
        placeKnob(trebleSlider, trebleLabel, boostControllerArea.removeFromLeft(verticalSectionWidth).reduced(10));
        placeKnob(symmetrySlider, symmetryLabel, boostControllerArea.removeFromLeft(verticalSectionWidth).reduced(10));

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

