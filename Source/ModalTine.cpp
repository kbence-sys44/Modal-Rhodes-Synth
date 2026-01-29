/*
  ==============================================================================

    ModalTine.cpp
    Created: 18 Jan 2026 3:14:54pm
    Author:  kadar

  ==============================================================================
*/

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
   // oversampling.initProcessing(specs.maximumBlockSize);
   // tempBuffer.setSize(specs.numChannels, specs.maximumBlockSize);

   // juce::dsp::ProcessSpec oversampledSpecs = specs;
   // oversampledSpecs.sampleRate = sampleRate * oversampling.getOversamplingFactor();

  /*  this->sampleRate = oversampledSpecs.sampleRate;

    //harmonikusok
    modes[0].frequencyRatio = 0.5f; 
    modes[1].frequencyRatio = 1.0f;//alaphang
    modes[2].frequencyRatio = 2.0f;
    modes[3].frequencyRatio = 3.0f;
    modes[4].frequencyRatio = 4.0f;
    modes[5].frequencyRatio = 5.0f;

    //inharmonikusok
    modes[6].frequencyRatio = 6.27f; //todo - ez frekvencia alapjan mas 
    modes[7].frequencyRatio = 17.55f;
    modes[8].frequencyRatio = 34.39f;

    //karakter, csillogas
    modes[9].frequencyRatio = 7.0f;
    modes[10].frequencyRatio = 8.0f;
    modes[11].frequencyRatio = 9.0f;
    modes[12].frequencyRatio = 10.0f;
    modes[13].frequencyRatio = 12.0f;
    modes[14].frequencyRatio = 14.0f;
    modes[15].frequencyRatio = 16.0f;

    //fix frekvencia
    modes[16].frequencyRatio = 0.25f;

    for (auto& mode : modes) {
        mode.prepare(oversampledSpecs);
    }
    */
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
    

    /*for (auto& mode : modes) {
        mode.filter.reset();
    }
    float baseDecay = juce::jmap(frequency, 40.0f, 2000.0f, 8.0f, 1.5f);

    float velMultiplier = 0.5f + (velocity * 1.5f);

    configSpikes(0, frequency, 1.0f, 0.9f * velMultiplier, 1.3f); //c2
    configSpikes(1, frequency, baseDecay, 1.6f* velMultiplier, 1.0f); //c3
    configSpikes(2, frequency, baseDecay, 1.1f * velMultiplier, 0.8f); //c4
    configSpikes(3, frequency, baseDecay, 0.15f * velMultiplier, 0.6f); //g4
    configSpikes(4, frequency, baseDecay, 0.10f * velMultiplier, 0.6f); //c5
    configSpikes(5, frequency, baseDecay, 0.15f * velMultiplier, 0.4f); //e5

    configSpikes(6, frequency, baseDecay, 0.13f * velMultiplier, 0.3f); // 627
    configSpikes(7, frequency, 0.15f, 0.002f * velMultiplier, 0.7f); // 1755
    configSpikes(8, frequency, 0.08f, 0.001f * velMultiplier, 0.5f); // 3439

    //configSpikes(9, frequency, baseDecay, 0.10f, 0.3f); //a#5
    //configSpikes(10, frequency, 0.25f, 0.05f, 1.0f); //c6
    //configSpikes(11, frequency, 0.20f, 0.05f, 1.0f); //d6
    //configSpikes(12, frequency, 0.18f, 0.04f, 1.0f); //e6
    //configSpikes(13, frequency, 0.15f, 0.03f, 1.0f); //g6
    //configSpikes(14, frequency, 0.12f, 0.015f, 1.0f); //a#6
    //configSpikes(15, frequency, 0.10f, 0.005f, 1.0f); //c7

    configSpikes(16, frequency, baseDecay * 2, 0.85f, 1.0f);

    globalVolumeAdjust = juce::jmap(frequency, 100.0f, 2300.0f, 1.0f, 0.3f);
    globalVolumeAdjust = juce::jlimit(0.1f, 1.0f, globalVolumeAdjust);
    DBG(globalVolumeAdjust);
    DBG(frequency);*/

    //reset();
}
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

/*    tempBuffer.setSample(0, 0, inputSample);

    juce::dsp::AudioBlock<float> block(tempBuffer);
    auto singleSampleBlock = block.getSubBlock(0, 1);

    //skalazas
    juce::dsp::AudioBlock<float> oversampledBlock = oversampling.processSamplesUp(singleSampleBlock);
    
    float* channelData = oversampledBlock.getChannelPointer(0);

    for (size_t i = 0; i < oversampledBlock.getNumSamples(); ++i) {
        float oIn = channelData[i];
        float oOut = 0.0f;

        for (auto& mode : modes) {
            if (mode.gain > 0.0001f) {
                float filtered = mode.filter.processSample(0, oIn);
                oOut += filtered * mode.gain;
            }
        }

        channelData[i] = oOut;
    }

    oversampling.processSamplesDown(singleSampleBlock);
       
    return tempBuffer.getSample(0, 0);*/
}

void ModalTine::setupMode(float frequency, float decayMult, float tone) {

    const float ratios[] = {0.25f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.27f, 17.55f, 34.39f };
    const float decayRatios[] = {1.0f, 1.0f, 1.0f, 0.7f, 0.6f, 0.5f, 0.4f, 0.5f, 0.1f, 0.05f};
    float gains[] = {0.02f * tone, 0.016f *tone, 0.75f* tone, 0.09f * tone , 0.4f * tone, 0.45f * tone , 0.10f * tone ,1.7f * tone, 6.0f * tone, 12.0f * tone };

    size_t limit = std::min(modes.size(), sizeof(ratios) / sizeof(ratios[0]));

    for (size_t i = 0; i < modes.size(); ++i)
    {
        if (frequency * ratios[i] > sampleRate * 0.30f) { //niquist
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

/*void ModalTine::configSpikes(int index, float frequency, float decay, float gain, float QMulti) {

    float f = frequency * modes[index].frequencyRatio;

    if (f > sampleRate * 0.48f) {
        modes[index].gain = 0.0f;
        return;
    }

    modes[index].filter.setCutoffFrequency(f);
    modes[index].filter.setResonance(calculateQ(f,decay) * QMulti);
    modes[index].gain = gain;

}

float ModalTine::calculateQ(float frequency, float decayTime) {
    if (decayTime < 0.001f) decayTime = 0.001f;
    if (frequency < 20.0f) frequency = 20.0f;

    float rad = 2.0f * juce::MathConstants<float>::pi * frequency;

    float qMultiplier = juce::jmap(frequency, 40.0f, 1500.0f, 0.1f, 1.0f);
    float q = decayTime * rad / 6.91f * qMultiplier;

    return juce::jlimit(0.5f, 800.0f, q);
}
*/