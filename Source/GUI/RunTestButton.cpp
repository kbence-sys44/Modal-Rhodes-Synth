/*
  ==============================================================================

    RunTestButton.cpp
    Created: 8 Feb 2026 11:36:28am
    Author:  kadar

  ==============================================================================
*/

#include "RunTestButton.h"

void RunTestButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsMouseOver, bool shouldDrawButtonAsDown) {

    auto bounds = getLocalBounds().toFloat().reduced(4.0f);

    juce::Colour baseColour = buttonColour;

    if (shouldDrawButtonAsMouseOver && getToggleState()) baseColour = buttonColour.brighter(0.2f);

    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, 3.0f);

    g.setColour(baseColour.darker(0.5f));
    g.drawRoundedRectangle(bounds, 3.0f, 3.0f);

    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText(getButtonText(), bounds, juce::Justification::centred, true);

}