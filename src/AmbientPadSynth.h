#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>


struct AmbientParams
{
    float activity = 0.0f;      // 0..1  (overall energy)
    float density  = 0.5f;      // chord thickness
    float warmth   = 0.5f;      // filter openness
    float space    = 0.5f;      // reverb amount
};

class AmbientPadSynth
{
public:
    void prepare(double sampleRate);
    void setParams(const AmbientParams& newParams);
    void process(float* left, float* right, int numSamples);

private:
    // ===== Core timing =====
    double sr = 48000.0;
    uint64_t sampleCounter = 0;

    // ===== Parameters (smoothed) =====
    AmbientParams target {};
    AmbientParams current {};

    // ===== Oscillators =====
    static constexpr int maxVoices = 4;
    float phase[maxVoices] {};
    float freq[maxVoices] {};

    // ===== DSP =====
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::dsp::Reverb reverb;

    // ===== Helpers =====
    float osc(int v);
    void updateChord();
};
