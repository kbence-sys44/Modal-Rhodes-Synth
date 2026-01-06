/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RhodesDWMAudioProcessorEditor::RhodesDWMAudioProcessorEditor (RhodesDWMAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    keyboardComponent(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    //ablak merete
    setSize (900, 500);

    //teszt elemek
    addAndMakeVisible(runTestsButton);
    runTestsButton.onClick = [this] {
        runner.runAllTests();
        textResults.setText(runner.getFullResults());
    };

    addAndMakeVisible(textResults);
    textResults.setMultiLine(true);
    textResults.setReadOnly(true);
    textResults.setReturnKeyStartsNewLine(true);

    //keyboard
    keyboardComponent.setAvailableRange(48, 108);
    keyboardComponent.setKeyWidth(24.45);
    addAndMakeVisible(keyboardComponent);
    
}

RhodesDWMAudioProcessorEditor::~RhodesDWMAudioProcessorEditor()
{
}

//==============================================================================
void RhodesDWMAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void RhodesDWMAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    runTestsButton.setBounds(area.removeFromTop(40));
    textResults.setBounds(area.removeFromTop(getHeight() / 3).withTrimmedTop(10));

    keyboardComponent.setBounds(area.removeFromBottom(getHeight() / 4).withTrimmedBottom(10));

}
