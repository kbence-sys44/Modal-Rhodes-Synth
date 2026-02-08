/*
  ==============================================================================

    LevelMeter.cpp
    Created: 4 Feb 2026 7:17:15pm
    Author:  kadar

  ==============================================================================
*/

#include "LevelMeter.h"


void LevelMeter::paint(juce::Graphics& g) {
    
    auto bounds = getLocalBounds().toFloat();

    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    float db = juce::Decibels::gainToDecibels(level, -60.0f);
    float normalized = juce::jmap(db, -60.0f, 6.0f, 0.0f, 1.0f);
    normalized = juce::jlimit(0.0f, 1.0f, normalized);

    float height = bounds.getHeight() * normalized;

    juce::ColourGradient grad(lowVolColour, bounds.getBottomLeft(),
        highVolColour, bounds.getTopLeft(), false);
    grad.addColour(0.75, mediumVolColour);

    g.setGradientFill(grad);
    g.fillRoundedRectangle(bounds.getX(), bounds.getBottom() - height, bounds.getWidth(), height, 4.0f);

    float y0 = bounds.getBottom() - (bounds.getHeight() * juce::jmap(0.0f, -60.0f, 6.0f, 0.0f, 1.0f));
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawLine(bounds.getX(), y0, bounds.getRight(), y0, 1.0f);

}

void LevelMeter::setLevel(float newLevel) {

    if (newLevel > level) {
        level = newLevel;
    }
    else {
        level = level * 0.9f; //decay
    }

    if (level < 0.0001f) level = 0.0f;

    repaint();
}