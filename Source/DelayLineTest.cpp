/*
  ==============================================================================

    DelayLineTest.cpp
    Created: 22 Dec 2025 4:32:01pm
    Author:  kadar

  ==============================================================================
*/


#include <JuceHeader.h>
#include "DelayLineTest.h"



    void DelayLineTest::runTest()
    {
        //teszt adatok
        juce::dsp::ProcessSpec specifications;
        specifications.sampleRate = 44100.0;
        specifications.maximumBlockSize = 512;
        specifications.numChannels = 1;

        beginTest("Delay Time Test");
        {
            DelayLineModule dl;
            dl.prepareDelayLine(specifications);


            //sample kesleltetes
            const int  testDelay = 50;
            dl.setDelayForDelayLine((float)testDelay);

            std::vector<float> inputSignal(testDelay + 50, 0.0f);
            //a 0-dik minta egy magas impulzus
            inputSignal[0] = 1.0f;

            //keslelteto vonal
            std::vector<float> outputSignal(testDelay + 50, 0.0f);

            for (int i = 0; i < outputSignal.size(); ++i) {
                outputSignal[i] = dl.processSample(inputSignal[i], 0.0f);
            }

            expectEquals(outputSignal[testDelay - 1], 0.0f, "Kesleltetes elott 0");
            expectEquals(outputSignal[testDelay], 1.0f, "Kesleltetes pontjan 1.0");
            expectEquals(outputSignal[testDelay + 1], 0.0f, "Kesleltetes utan 0");
        }

        beginTest("Frakcionalis kesleltetes teszt");
        {
            DelayLineModule dl;
            dl.prepareDelayLine(specifications);

            //100.5 sample kesleltetes
            const float  testDelay = 100.5f;
            dl.setDelayForDelayLine(testDelay);

            float inputSignal[150] = { 0.0f };
            inputSignal[0] = 1.0f;

            float outputSignal[150] = { 0.0f };

            for (int i = 0; i < 150; ++i) {
                outputSignal[i] = dl.processSample(inputSignal[i], 0.0f);
            }

            expectWithinAbsoluteError(outputSignal[100], 0.0f, 0.5f, "100-dik mintan nincs teljes csucs");
            expectWithinAbsoluteError(outputSignal[101], 0.5f, 0.5f, "101-dik mintan van a csucs egy resze");
            expectGreaterThan(outputSignal[100], 0.001f, "frakcionalis kesleltetes tortent");
        }

        beginTest("Feedback Loop Damping Test");
        {
            DelayLineModule dl;
            dl.prepareDelayLine(specifications);

            //100.5 sample kesleltetes
            const int  testDelay = 100;
            dl.setDelayForDelayLine((float)testDelay);

            float inputSample = 1.0f;
            float outputSample = 0.0f;

            float feedback = 0.99f;

            for (int i = 0; i < 1000; ++i) {
                //csak az elso sample kap impulzust
                outputSample = dl.processSample(i == 0 ? inputSample : 0.0f, feedback);
                //DBG(std::to_string(outputSample));
            }

            expectLessThan(std::abs(outputSample), 1.0f, "A feedback jelnek csillapodnia kell.");
            expectGreaterThan(std::abs(outputSample), 0.0000001f, "A jel nem halhat meg teljesen.");
        }

}

DelayLineTest delayLineTestInstance;