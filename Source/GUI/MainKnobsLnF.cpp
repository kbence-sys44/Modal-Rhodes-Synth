/*
  ==============================================================================

    mainKnobs.cpp
    Created: 4 Feb 2026 8:34:46pm
    Author:  kadar

  ==============================================================================
*/

#include "MainKnobsLnF.h"

void MainKnobsLnF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosition,
    const float rotationStartAngle, const float rotationEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(2.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centre = bounds.getCentre();
    auto toAngle = rotationStartAngle + sliderPosition * (rotationEndAngle - rotationStartAngle);


    //kulso vonalak
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    for (float ang = rotationStartAngle; ang <= rotationEndAngle; ang += (juce::MathConstants<float>::twoPi / 45.0f)) {

        juce::Path tick;
        tick.startNewSubPath(centre.getPointOnCircumference(radius, ang));
        tick.lineTo(centre.getPointOnCircumference(radius - 10.0f, ang));
        g.strokePath(tick, juce::PathStrokeType(1.0f));
    }

    float knobRadius = radius * 0.80f; //gomb kisebb


    juce::ColourGradient ringGradient(juce::Colour(0xff4b4b4b), centre.x, centre.y - knobRadius, juce::Colour(0xff2a2a2a), centre.x, centre.y + knobRadius, false);
    g.setGradientFill(ringGradient);
    g.fillEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);
    
    //bordak
    g.setColour(juce::Colour(0xff1a1a1a));
    int numberOfRidges = 40;
    for (int i = 0; i < numberOfRidges; ++i) {

        float angle = (float)i / (float)numberOfRidges * juce::MathConstants<float>::twoPi;
        juce::Path ridge;

        auto p1 = centre.getPointOnCircumference(knobRadius, angle - 0.02f);
        auto p2 = centre.getPointOnCircumference(knobRadius, angle + 0.02f);
        auto p3 = centre.getPointOnCircumference(knobRadius * 0.6f, angle);

        ridge.startNewSubPath(p1);
        ridge.lineTo(p2);
        ridge.lineTo(p3);
        ridge.closeSubPath();
        g.fillPath(ridge);

    }

    //keret
    g.setColour(juce::Colours::black);
    g.drawEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.5f);

    //belso resz
    float faceRadius = knobRadius * 0.5f;

    juce::ColourGradient faceGradient(juce::Colour(250, 250, 250), centre.x - faceRadius, centre.y - faceRadius, juce::Colour(227, 242, 253), centre.x + faceRadius, centre.y + faceRadius, false);

    g.setGradientFill(faceGradient);
    g.fillEllipse(centre.x - faceRadius, centre.y - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f);

    //elva	zto
    g.setColour(juce::Colour(juce::Colours::black));
    g.drawEllipse(centre.x - faceRadius, centre.y - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f, 2.0f);


    //mutato
    juce::Path path;
    float pointerLength = faceRadius * 0.5f;
    float pointerThickness = 3.0f;
    path.addRectangle(-pointerThickness / 2.0f, -faceRadius * 0.45f, pointerThickness, -pointerLength);
    path.applyTransform(juce::AffineTransform::rotation(toAngle).translated(centre.x, centre.y));

    juce::Path shadowP = path;
    shadowP.applyTransform(juce::AffineTransform::translation(1.0f, 1.0f));
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillPath(shadowP);

    g.setColour(juce::Colours::black.withAlpha(0.85f));
    g.fillPath(path);


    //mouse down ertek kiiras
    /*
    if (slider.isMouseButtonDown()) {
        g.setColour(juce::Colours::black.withAlpha(0.7f));
        //g.fillRoundedRectangle(centre.x - 20, centre.y - 10, 40, 20, 4.0f);

        g.setColour(juce::Colours::darkslategrey);
        g.setFont(13.0f);

        juce::String text = juce::String(slider.getValue(), 1);
        if (slider.getTextValueSuffix().isNotEmpty()) {
            text += slider.getTextValueSuffix();
        }

        g.drawText(text, x, y, width, height, juce::Justification::centred, false);

    }*/
}