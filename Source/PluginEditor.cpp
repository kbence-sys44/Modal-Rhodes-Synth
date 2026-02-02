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

    //keyboard
    keyboardComponent.setAvailableRange(48, 96);
    keyboardComponent.setKeyWidth(30.45);
    addAndMakeVisible(keyboardComponent);

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

    driveSlider.setSliderStyle(juce::Slider::LinearVertical);
    outputSlider.setSliderStyle(juce::Slider::LinearVertical);

    addAndMakeVisible(tremoloToggleButton);
    tremoloToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "TREM_SWITH", tremoloToggleButton);

    addAndMakeVisible(reverbToggleButton);
    reverbToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "REVERB_SWITCH", reverbToggleButton);

    addAndMakeVisible(cabinetToggleButton);
    cabinetToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "CABINET_SWITCH", cabinetToggleButton);

    addAndMakeVisible(delayToggleButton);
    cabinetToggleAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "DELAY_SWITCH", delayToggleButton);

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
        &tremoloToggleButton, &reverbToggleButton, &cabinetToggleButton, &delayToggleButton
        
    };

    for (auto* all : standardControls) {
        all->setVisible(normalUI);
    }

}

//==============================================================================
void ModalRhodesAudioProcessorEditor::paint(juce::Graphics& g)
{
    //hatter
    g.fillAll(juce::Colour(0xff2b2b2b));

    //elvalaszto vonal
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    g.drawLine(20, 50, getWidth() - 20, 50, 1.0f);

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

}

//elrendezes
void ModalRhodesAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    auto headerArea = area.removeFromTop(50);
    titleLabel.setBounds(headerArea);

    debugButton.setBounds(headerArea.removeFromRight(110).reduced(20, 10));

    if (debugButton.getToggleState()) {
        auto debugArea = area.reduced(20);

        runTestsButton.setBounds(debugArea.removeFromTop(40));
        debugArea.removeFromTop(10);

        textResults.setBounds(debugArea);
    }
    else {
        //normal mod
        auto placeKnob = [](juce::Slider& slider, juce::Label& label, juce::Rectangle<int> bounds) {
            auto labelBounds = bounds.removeFromBottom(15);
            bounds.removeFromBottom(5);
            slider.setBounds(bounds);
            label.setBounds(labelBounds);
           };

        area = area.reduced(10);
        area.removeFromBottom(10);
        area.removeFromLeft(10);
        area.removeFromRight(10);

        keyboardComponent.setBounds(area.removeFromBottom(150));
        area.removeFromBottom(10);

        auto controlHeight = (area.getHeight() - 20) / 2;
        auto controlWidth = area.getWidth() / 8;

        //bal oldal
        auto leftSide = area.removeFromLeft(controlWidth * 3);

        auto cabinetArea = leftSide.removeFromTop(20);
        cabinetLabel.setBounds(cabinetArea); // ez egy gomb lesz 

        mainSectionBounds = leftSide;

        auto leftSideTop = leftSide.removeFromTop(controlHeight);
        auto leftSideBottom = leftSide.removeFromBottom(controlHeight);

        placeKnob(hardnessSlider, hardnessLabel, leftSideTop.removeFromLeft(controlWidth).reduced(10));
        placeKnob(decaySlider, decayLabel, leftSideTop.removeFromLeft(controlWidth).reduced(10));
        placeKnob(releaseSlider, releaseLabel, leftSideTop.removeFromLeft(controlWidth).reduced(10));

        placeKnob(bassSlider, bassLabel, leftSideBottom.removeFromLeft(controlWidth).reduced(10));
        placeKnob(trebleSlider, trebleLabel, leftSideBottom.removeFromLeft(controlWidth).reduced(10));
        placeKnob(symmetrySlider, symmetryLabel, leftSideBottom.removeFromLeft(controlWidth).reduced(10));

        //in out
        auto rightSection1 = area.removeFromLeft(controlWidth);
        auto linearSliderAreaWidth = rightSection1.getWidth() / 2;

        auto linearSlider1 = rightSection1.removeFromLeft(linearSliderAreaWidth);
        auto linearHeader1 = linearSlider1.removeFromTop(20);
        inLabel.setBounds(linearHeader1);
        linearSlider1.removeFromBottom(10);
        driveSlider.setBounds(linearSlider1);

        auto linearSlider2 = rightSection1.removeFromLeft(linearSliderAreaWidth);
        auto linearHeader2 = linearSlider2.removeFromTop(20);
        outLabel.setBounds(linearHeader2);
        linearSlider2.removeFromBottom(10);
        outputSlider.setBounds(linearSlider2);

        //reverb
        auto rightSection2 = area.removeFromLeft(controlWidth);
        auto rightSection2Header = rightSection2.removeFromTop(20);

        reverbBounds = rightSection2;

        auto rightSectionControlHeight = rightSection2.getHeight() / 2;

        auto reverbButtonArea = rightSection2Header.removeFromLeft(20);
        reverbToggleButton.setBounds(reverbButtonArea.withSizeKeepingCentre(20, 20));

        rightSection2Header.removeFromRight(20);
        reverbLabel.setBounds(rightSection2Header);

        placeKnob(reverbDrySlider, reverbDryLabel, rightSection2.removeFromTop(rightSectionControlHeight).reduced(10));
        placeKnob(reverbWetSlider, reverbWetLabel, rightSection2.removeFromTop(rightSectionControlHeight).reduced(10));


        //tremolo
        auto rightSection3 = area.removeFromLeft(controlWidth);
        auto rightSection3Header = rightSection3.removeFromTop(20);

        tremoloBounds = rightSection3;

        auto tremoloButtonArea = rightSection3Header.removeFromLeft(20);
        tremoloToggleButton.setBounds(tremoloButtonArea.withSizeKeepingCentre(20, 20));

        rightSection3Header.removeFromRight(20);
        tremoloLabel.setBounds(rightSection3Header);

        placeKnob(tremDepthSlider, tremDepthLabel, rightSection3.removeFromTop(rightSectionControlHeight).reduced(10));
        placeKnob(tremRateSlider, tremRateLabel, rightSection3.removeFromTop(rightSectionControlHeight).reduced(10));

        //delay
        auto rightSection4 = area.removeFromLeft(controlWidth);
        auto rightSection4Header = rightSection4.removeFromTop(20);

        delayBounds = rightSection4;

        auto delayButtonArea = rightSection4Header.removeFromLeft(20);
        delayToggleButton.setBounds(delayButtonArea.withSizeKeepingCentre(20, 20));

        rightSection4Header.removeFromRight(20);
        delayLabel.setBounds(rightSection4Header);

        //placeKnob(, , rightSection3.removeFromTop(rightSectionControlHeight).reduced(10));
        //placeKnob(, , rightSection3.removeFromTop(rightSectionControlHeight).reduced(10));
    }
}

