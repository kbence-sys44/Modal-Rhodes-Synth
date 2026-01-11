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
        /*
        beginTest("Delay Time Test");
        {
            DelayLineModule dl;
            dl.prepareDelayLine(specifications);


            //sample kesleltetes
            const int  testDelay = 50;
            dl.setDelayForDelayLine((float)testDelay, 1.0f);

            std::vector<float> inputSignal(testDelay + 50, 0.0f);
            //a 0-dik minta egy magas impulzus
            inputSignal[0] = 1.0f;

            //keslelteto vonal
            std::vector<float> outputSignal(testDelay + 50, 0.0f);

            int maxI = -1;
            float maxValue = 0.0f;

            for (int i = 0; i < outputSignal.size(); ++i) {
                outputSignal[i] = dl.processSample(inputSignal[i], 0.0f);

                if (outputSignal[i] > maxValue) {
                    maxValue = outputSignal[i];
                    maxI = i;
                }
            }

            bool isPeakRight = (maxI >= testDelay - 3 && maxI <= testDelay);

            expect(isPeakRight, "A kesleltetes nem vart tartomanyba esik. Kapott index: " + juce::String(maxI));

            expectGreaterThan(maxValue, 0.1f, "A jel tul halk");
        }

        beginTest("Frakcionalis kesleltetes teszt");
        {
            DelayLineModule dl;
            dl.prepareDelayLine(specifications);

            //100.5 sample kesleltetes
            const float  testDelay = 100.5f;
            dl.setDelayForDelayLine(testDelay, 1.0f);

            float inputSignal[150] = { 0.0f };
            inputSignal[0] = 1.0f;

            int maxI = -1;
            float maxValue = 0.0f;

            for (int i = 0; i < 150; ++i) {
                float outputSignal = dl.processSample(inputSignal[i], 0.0f);

                if (outputSignal > maxValue) {
                    maxValue = outputSignal;
                    maxI = i;
                }
            }

            bool isPeakRight = (maxI >= 98 && maxI <= 101);

            expect(isPeakRight, "A frakcionalis kesleltetes csucsa rossz helyen van. Index: " + juce::String(maxI));
        }

        beginTest("Feedback Loop Damping Test");
        {
            DelayLineModule dl;
            dl.prepareDelayLine(specifications);

            //100.5 sample kesleltetes
            const int  testDelay = 100;
            dl.setDelayForDelayLine((float)testDelay, 1.0f);

            float inputSample = 1.0f;
            float outputSample = 0.0f;

            float feedback = 0.99f;

            for (int i = 0; i < 1000; ++i) {
                //csak az elso sample kap impulzust
                outputSample = dl.processSample(i == 0 ? inputSample : 0.0f, feedback);
                //DBG(std::to_string(outputSample));
            }

            expectLessThan(std::abs(outputSample), 0.9f, "A feedback jelnek csillapodnia kell.");
            expectGreaterThan(std::abs(outputSample), 0.0000001f, "A jel nem halhat meg teljesen.");
        }*/

}

DelayLineTest delayLineTestInstance;