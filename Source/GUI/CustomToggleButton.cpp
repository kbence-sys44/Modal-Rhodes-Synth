/*
  ==============================================================================

    CustomToggleButton.cpp
    Created: 31 Jan 2026 9:27:06pm
    Author:  kadar

  ==============================================================================
*/

#include "CustomToggleButton.h"

void CustomToggleButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsMouseOver, bool shouldDrawButtonAsDown) {

    auto bounds = getLocalBounds().toFloat().reduced(4.0f);

    juce::Colour baseColour = getToggleState() ? juce::Colours::red : juce::Colours::darkred.darker(0.8f);

    if (shouldDrawButtonAsMouseOver) baseColour = baseColour.brighter(0.2f);

    g.setColour(baseColour);
    g.fillEllipse(bounds);

    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawEllipse(bounds, 1.0f);

    if (getToggleState()) {
        g.setGradientFill(juce::ColourGradient(juce::Colours::white.withAlpha(0.4f), bounds.getCentre(), baseColour, bounds.getTopLeft(), true));
        g.fillEllipse(bounds);
    }

}