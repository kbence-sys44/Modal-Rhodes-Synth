/*
  ==============================================================================

    CrossleyHammer.cpp
    Created: 20 Jan 2026 5:50:40pm
    Author:  kadar

  ==============================================================================
*/

#include "CrossleyHammer.h"
#include <cmath>

void CrossleyHammer::prepareHammer(const juce::dsp::ProcessSpec& specs) {
    sampleRate = specs.sampleRate;
    active = false;
    force = 0.0f;
    hammerPos = -0.01f; //1cm
    thumpFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 300.0f);
    thumpFilter.reset();
}

void CrossleyHammer::triggerHammer(float velocity, float delayInSamples, int noteNum) {

    float initialSpeed = velocity * 5.0f;

    hammerPos = -0.005f;
    hammerVel = initialSpeed;

    travelDelayCounter = static_cast<int>(delayInSamples);
    if (travelDelayCounter < 0) travelDelayCounter = 0;

    random.setSeed(noteNum * 12345);
    for (int i = 0; i < 10; ++i) random.nextFloat();

    force = 0.0f;
    active = true;

    thumpFilter.reset();

}

float CrossleyHammer::getNextSample() {

    if (!active) return 0.0f;

    if (travelDelayCounter > 0) { // meg nem ert oda
        travelDelayCounter--;
        return 0.0f;
    }

    float dt = 1.0f / (float)sampleRate; //euler integracio
    hammerPos += hammerVel * dt;

    if (hammerPos > 0.0f) { //kalapacs elerte a femet
        float compression = hammerPos;
        float compressionVel = hammerVel;

        //hunt-crossley F=K*x^p*(1+lambda*v)
        //minel gyorsabb az utes, annal kemenyebbnek tunik az anyag

        float stiffnessForce = stiffness * std::pow(compression, exponent);
        float dampForce = 1.0f + (damping * compressionVel);

        if (dampForce < 0.0f) dampForce = 0.0f; //nem lehet negativ

        force = stiffnessForce * dampForce;

        float acc = -force / mass; //Newton III. F=m*a

        hammerVel += acc * dt;

    }
    else {//nincs kontakt
        force = 0.0f;

        if (hammerVel < 0.0f && hammerPos < -0.005f) { //visszatert a kalapacs
            active = false;
        }

    }

    return force * 0.004f;
}

bool CrossleyHammer::isHammerActive() const {
    return active;
}

float CrossleyHammer::getThump() {
    float noise = (random.nextFloat() * 2.0f) - 1.0f;

    float noiseBurst = noise * force * 0.5f;

    return thumpFilter.processSample(noiseBurst);
}

void CrossleyHammer::setParameters(float newStiffnes, float newMass) {
    stiffness = newStiffnes;
    mass = newMass;
}