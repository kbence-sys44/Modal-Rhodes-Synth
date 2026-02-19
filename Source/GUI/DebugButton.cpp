/*
  ==============================================================================

    DebugButton.cpp
    Created: 8 Feb 2026 11:17:38am
    Author:  kadar

  ==============================================================================
*/

#include "DebugButton.h"

void DebugButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsMouseOver, bool shouldDrawButtonAsDown) {

    auto bounds = getLocalBounds().toFloat().reduced(4.0f);

    juce::Colour baseColour = getToggleState() ? juce::Colours::darkgrey : juce::Colours::darkgrey.darker(0.8f);

    if (shouldDrawButtonAsMouseOver) baseColour = baseColour.brighter(0.2f);

    g.setColour(baseColour);
    g.fillEllipse(bounds);

    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.drawEllipse(bounds, 1.0f);

    if (getToggleState()) {
        g.setGradientFill(juce::ColourGradient(juce::Colours::black.withAlpha(0.4f), bounds.getCentre(), baseColour, bounds.getTopLeft(), true));
        g.fillEllipse(bounds);
    }

}