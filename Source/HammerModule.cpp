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

    hammerFilter.prepare({ sRate, 512, 1 });
    hammerFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    hammerFilter.setResonance(0.5f);

    thumpFilter.prepare({ sRate, 512, 1 });
    thumpFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    thumpFilter.setCutoffFrequency(220.0f);
    thumpFilter.setResonance(0.3f); //tompa
}

//utes elinditasa
void HammerModule::triggerHammer(float velocity, float length, int noteNum) {

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
    float velocityBrightness = 0.1f + (velocity*0.8f);
    float  noteFreq = sampleRate / length;
    float baseFreq = 0.0f;
    if (noteFreq > 500.0f) {
        baseFreq = (8000.0f * velocityBrightness) + (noteFreq * 2.0f);
    }
    else {
        baseFreq = 3000.0f * velocityBrightness;
    }
    baseFreq = juce::jlimit(200.0f, 16000.0f, baseFreq);

    hammerFilter.setCutoffFrequency(baseFreq);
    hammerFilter.reset();


    thumpEnv = velocity * 4.0f;
    float thumpDur = 0.02f + (0.05f * (1.0f - velocity));
    thumpDecay = std::pow(0.01f, 1.0f / (sampleRate * thumpDur));

    

    
    thumpFilter.reset();

}


//utes lezajlasaert felelos fuggveny
float HammerModule::getNextSample() {
    if (remainingSamples <= 0) return 0.0f;

    //burkologorbe
    float pos = 1.0f - (static_cast<float>(remainingSamples) / static_cast<float>(totalDur));
    float env = std::sin(pos * juce::MathConstants<float>::pi);
    remainingSamples--;
;

    float hardness = 1.0f + currentVelocity * 8.0f;
    float noise = (random.nextFloat() * 2.0f - 1.0f) * 0.5f;

    env = std::pow(env, hardness);
    float noiseMix = noise * 0.05f;

    return (env + noiseMix) * currentVelocity;
}

bool HammerModule::isHammerActive() const {
    return (remainingSamples > 0);
}

float HammerModule::getVelocity() const{
    return currentVelocity;
}

float HammerModule::getThump() {
    if (thumpEnv < 0.001f) return 0.0f;

    float rawNoise = (random.nextFloat() * 2.0f) - 1.0f;
    float thump = thumpFilter.processSample(0, rawNoise);
    float out = thump * thumpEnv;

    thumpEnv *= thumpDecay;

    return out;
}