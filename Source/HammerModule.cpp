/*
  ==============================================================================

    HammerModule.cpp
    Created: 23 Dec 2025 4:16:00pm
    Author:  kadar

  ==============================================================================
*/

#include "HammerModule.h"

void HammerModule::prepareHammer(double sRate) {
    sampleRate = sRate;
    random.setSeedRandomly();
}

//utes elinditasa
void HammerModule::triggerHammer(float velocity, float length) {

    //a kalapacs hossza kb 60%-a hurnak
    float baseLength = length * 0.6f;
    float velFactor = juce::jmap(velocity, 0.0f, 1.0f, 1.0f, 0.7f);
    int dynamicLength = static_cast<int>(baseLength * velFactor);

    int minLen = static_cast<int>(0.001 * sampleRate);
    int maxLen = static_cast<int>(0.020 * sampleRate);

    totalDur = juce::jlimit(minLen, maxLen, dynamicLength);
    remainingSamples = totalDur;

    currentVelocity = velocity; 

    //kicsi vel -> puha kalapacs, nagy vel -> kemenyebb
    float velocityBrigtness = 0.4f * (velocity*0.58f);
    //pitch alapu tompitas
    float pitchDamping = juce::jmap(length, 100.0f, 1000.0f, 0.0f, 0.05f);

    filterCoefficient = juce::jlimit(0.1f, 0.99f, velocityBrigtness - pitchDamping);
}


//utes lezajlasaert felelos fuggveny
float HammerModule::getNextSample() {
    if (remainingSamples <= 0) return 0.0f;

    //burkologorbe
    float pos = 1.0f - (static_cast<float>(remainingSamples) / static_cast<float>(totalDur));
    float env = std::sin(pos * juce::MathConstants<float>::pi);

    remainingSamples--;

    float rawNoise = (random.nextFloat() * 2.0f - 1.0f);
    
    float filteredNoise = (lastOutput * 0.9f) + (rawNoise * 0.1f * (1.0f + currentVelocity));
    lastOutput = filteredNoise;

    float clickAmount = currentVelocity * currentVelocity * currentVelocity;
    float click = rawNoise * 0.4f * clickAmount;

    float output = (filteredNoise + click) * env * currentVelocity;

    return output * 2.0f;
}

bool HammerModule::isHammerActive() const {
    return (remainingSamples > 0);
}