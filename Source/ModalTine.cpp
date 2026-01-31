/*
  ==============================================================================

    ModalTine.cpp
    Created: 18 Jan 2026 3:14:54pm
    Author:  kadar

  ==============================================================================
*/

//ez a legfontosabb modul, ez felel a rezonator rud es a tine rezgeseiert
//a modalis megkozelites miatt nem egy hullamot general, hanem tobb kulonallo rugo rendszert futtat egyszerre
//a kalapacstol kapott erobol minden modusz kiszamolja a sajat kovetkezo allapotat es a kimenet a moduszok elmozdulasa lesz

#include "ModalTine.h"

ModalTine::ModalTine() {
    modes.resize(10);
}

ModalTine::~ModalTine(){}

void ModalTine::prepare(const juce::dsp::ProcessSpec& specs) {
    this->sampleRate = specs.sampleRate;
    dt = 1.0f / (float)sampleRate;

    displacement = 0.0f;

    for (auto& m: modes )
    {
        m.displacement = 0.0f;
        m.velocity = 0.0f;
    }
}
void ModalTine::reset() {
    for (auto& mode : modes) {
        mode.displacement = 0.0f;
        mode.velocity = 0.0f;
    }
    displacement = 0.0f;
}
void ModalTine::setParams(float frequency, float decayTime, float tone) {
    setupMode(frequency, decayTime, tone);
    reset();

}

//elmozdulas kiszamitasa
float ModalTine::process(float inputForce) {
    displacement = 0.0f;

    if (inputForce > 1000.0f) inputForce = 1000.0f;
    if (inputForce < -1000.0f) inputForce = -1000.0f;

    for (auto& m : modes) {

        float springF = -(m.mass * m.omega * m.omega) * m.displacement;
        float dampF = -2.0f * m.mass * m.decay * m.velocity;

        float totalF = (inputForce * m.gain) + springF + dampF;
        float acc = totalF / m.mass;

        m.velocity += acc * dt;
        m.displacement += m.velocity * dt;
        
        if (std::abs(m.displacement) > 1.0f) {
            m.displacement = 0.0f;
            m.velocity = 0.0f;
        }

        displacement += m.displacement; 
    }

    if (std::isnan(displacement)) displacement = 0.0f;

    return displacement;
}

//minden modusznak szukseges adatok megadasa
void ModalTine::setupMode(float frequency, float decayMult, float tone) {

    const float ratios[] = {0.25f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.27f, 17.55f, 34.39f }; //felharmonikusok szorzoja
    const float decayRatios[] = {1.0f, 1.0f, 1.0f, 0.7f, 0.6f, 0.5f, 0.4f, 0.5f, 0.1f, 0.05f}; //a lecsenges annal kisebb minel magasabb frekvencia
    float gains[] = {0.02f * tone, 0.016f *tone, 0.75f* tone, 0.09f * tone , 0.4f * tone, 0.45f * tone , 0.10f * tone ,1.7f * tone, 6.0f * tone, 12.0f * tone }; //hangero

    size_t limit = std::min(modes.size(), sizeof(ratios) / sizeof(ratios[0]));

    for (size_t i = 0; i < modes.size(); ++i)
    {
        if (frequency * ratios[i] > sampleRate * 0.30f) { //niquist limit az osszeomlas ellen
            modes[i].gain = 0.0f;
            modes[i].omega = 0.0f;
            modes[i].decay = 1.0f;
        }
        else {
            modes[i].omega = 2.0f * juce::MathConstants<float>::pi * frequency * ratios[i];

            float tau = decayMult * decayRatios[i];
            modes[i].decay = 1.0f / (tau + 0.001f);
            modes[i].gain = gains[i];
            modes[i].mass = 0.001f;
        }

    }

}

void ModalTine::applyDamping(float release) {

    for (auto& m : modes) {
        m.decay = m.decay * (1.0f + release);
    }

}