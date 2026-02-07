/*
  ==============================================================================

    OnOffButton.cpp
    Created: 7 Feb 2026 12:50:29pm
    Author:  kadar

  ==============================================================================
*/

#include "OnOffButton.h"

void OnOffButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsMouseOver, bool shouldDrawButtonAsDown) {

    auto bounds = getLocalBounds().toFloat().reduced(4.0f);

    juce::Colour baseColour;
    if (buttonType == Type::Off) baseColour = getToggleState() ? offColour : offColour.darker(0.8f);
    if (buttonType == Type::On) baseColour = getToggleState() ? onColour : onColour.darker(0.8f);

    if (shouldDrawButtonAsMouseOver) baseColour = baseColour.brighter(0.2f);

    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, 3.0f);

    g.setColour(baseColour.darker(0.5f));
    g.drawRoundedRectangle(bounds, 3.0f, 3.0f);

    g.setColour(getToggleState() ? juce::Colours::white : juce::Colours::grey);
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText(getButtonText(), bounds, juce::Justification::centred, true);

}