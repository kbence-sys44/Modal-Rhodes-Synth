/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RhodesDWMAudioProcessorEditor::RhodesDWMAudioProcessorEditor (RhodesDWMAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //ablak mérete
    setSize (900, 600);

    addAndMakeVisible(runTestsButton);
    runTestsButton.onClick = [this] {
        runner.runAllTests();
        textResults.setText(runner.getFullResults());
    };

    addAndMakeVisible(textResults);
    textResults.setMultiLine(true);
    textResults.setReadOnly(true);
    textResults.setReturnKeyStartsNewLine(true);
    
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
    textResults.setBounds(area.removeFromTop(getHeight() / 2).withTrimmedTop(10));

}
